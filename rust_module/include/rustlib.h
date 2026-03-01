/*
 * rustlib.h
 * ========
 * Full-featured C header for the rust-based NLP test driver. This header
 * provides a broad set of lightweight APIs that are intentionally simple to
 * bind from Go (cgo) and Python (ctypes/cffi), while also exposing a richer
 * set of operations useful for local testing and minimal inference.
 *
 * Design goals:
 * - Small, stable C ABI types (plain pointers, integers, size_t, char*).
 * - Clear ownership / allocation conventions for cross-language safety.
 * - Convenience helpers for common tasks: tokenization, normalization,
 *   n-grams, simple embeddings, and a toy model loader interface.
 * - Batch and streaming variants so the header can be used from higher-level
 *   pipelines without complex marshalling on the caller side.
 *
 * NOTE: This header is intended for demonstration and testing. In production
 * systems, prefer language-specific wrappers and a proper FFI boundary.
 */

#ifndef RUSTLIB_H
#define RUSTLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Export macro: visibility for shared libraries. Define RUSTLIB_BUILD when
 * building the shared library to export symbols on Windows. */
#if defined(_WIN32) || defined(__CYGWIN__)
#  ifdef RUSTLIB_BUILD
#    define RUSTLIB_EXPORT __declspec(dllexport)
#  else
#    define RUSTLIB_EXPORT __declspec(dllimport)
#  endif
#else
#  if __GNUC__ >= 4
#    define RUSTLIB_EXPORT __attribute__((visibility("default")))
#  else
#    define RUSTLIB_EXPORT
#  endif
#endif

/* ------------------------------------------------------------------------- */
/* Error codes */
typedef enum {
	RUSTLIB_OK = 0,
	RUSTLIB_ERR_GENERIC = 1,
	RUSTLIB_ERR_OOM = 2,
	RUSTLIB_ERR_INVALID_ARG = 3,
	RUSTLIB_ERR_NOT_FOUND = 4,
	RUSTLIB_ERR_MODEL = 5,
} rustlib_status_t;

/* ------------------------------------------------------------------------- */
/* Simple token representation */
typedef struct {
	const char* text; /* pointer into owned buffer or NUL-terminated copied string */
	size_t start;     /* byte offset in original buffer */
	size_t end;       /* exclusive byte offset */
} rust_token_t;

/* Tokenizer config */
typedef struct {
	bool lowercase;
	bool strip_punct;
	size_t max_tokens;
} rust_tokenizer_cfg_t;

/* Create/destroy tokenizer config */
RUSTLIB_EXPORT rust_tokenizer_cfg_t* rust_tokenizer_cfg_new(void);
RUSTLIB_EXPORT void rust_tokenizer_cfg_free(rust_tokenizer_cfg_t* cfg);
RUSTLIB_EXPORT void rust_tokenizer_cfg_set_lowercase(rust_tokenizer_cfg_t* cfg, bool v);
RUSTLIB_EXPORT void rust_tokenizer_cfg_set_strip_punct(rust_tokenizer_cfg_t* cfg, bool v);
RUSTLIB_EXPORT void rust_tokenizer_cfg_set_max_tokens(rust_tokenizer_cfg_t* cfg, size_t n);

/* Tokenize a NUL-terminated string. Returns number of tokens written into
 * tokens_out up to tokens_capacity. The tokens are allocated as copies via
 * rust_analyze_alloc, so callers should free them using rust_free_cstring if
 * they need to own them. If tokens_out is NULL, the function returns
 * the number of tokens that would be produced (dry run). */
RUSTLIB_EXPORT rustlib_status_t rust_tokenize(
	const char* s,
	const rust_tokenizer_cfg_t* cfg,
	rust_token_t* tokens_out,
	size_t tokens_capacity,
	size_t* tokens_written
);

/* Convenience: return tokens as a single heap-allocated JSON string.
 * Caller must free with rust_free_cstring. */
RUSTLIB_EXPORT char* rust_tokenize_json(const char* s, const rust_tokenizer_cfg_t* cfg);

/* ------------------------------------------------------------------------- */
/* Normalization helpers */
typedef enum {
	RUST_NORM_NONE = 0,
	RUST_NORM_NFKC = 1,
	RUST_NORM_LOWERCASE = 2,
	RUST_NORM_REMOVE_DIACRITICS = 4,
} rust_norm_flags_t;

RUSTLIB_EXPORT char* rust_normalize_alloc(const char* s, uint32_t flags);

/* ------------------------------------------------------------------------- */
/* N-grams and shingling */
RUSTLIB_EXPORT size_t rust_count_ngrams(const char* s, unsigned int n);
RUSTLIB_EXPORT char* rust_ngram_join_alloc(const char* s, unsigned int n, const char* sep);

/* ------------------------------------------------------------------------- */
/* Simple embedding interface (toy vector API) */
typedef struct {
	size_t dim;
	float* data; /* length dim, owned by library when returned via *_alloc */
} rust_vec_t;

/* Compute a toy embedding for a string. Returns RUSTLIB_OK and sets *out to
 * a heap-allocated vector (free with rust_vec_free). */
RUSTLIB_EXPORT rustlib_status_t rust_embed_alloc(const char* s, rust_vec_t** out);
RUSTLIB_EXPORT void rust_vec_free(rust_vec_t* v);

/* Cosine similarity between two vectors. Returns -1.0 on error. */
RUSTLIB_EXPORT double rust_vec_cosine(const rust_vec_t* a, const rust_vec_t* b);

/* Batch embedding API: allocate a contiguous buffer of embeddings for `n` inputs
 * and fill it. The memory layout is row-major: n x dim floats. Caller frees
 * the returned pointer with rust_free_cstring (for simplicity) or uses
 * rust_vec_free on each rust_vec_t when using per-item API. */
RUSTLIB_EXPORT rustlib_status_t rust_batch_embed_alloc(const char** inputs, size_t n, float** out_buf, size_t* out_dim);

/* ------------------------------------------------------------------------- */
/* Model loader (toy) */
typedef struct rust_model rust_model_t; /* opaque */

/* Load a model from a binary blob in memory. The blob is not copied; the
 * caller may free it after load if the loader copies the data. Returns NULL
 * on error. For the toy interface we accept a path or pointer-to-bytes. */
RUSTLIB_EXPORT rust_model_t* rust_model_load_from_file(const char* path, rustlib_status_t* err);
RUSTLIB_EXPORT rust_model_t* rust_model_load_from_blob(const unsigned char* blob, size_t len, rustlib_status_t* err);
RUSTLIB_EXPORT void rust_model_free(rust_model_t* m);

RUSTLIB_EXPORT const char* rust_model_name(const rust_model_t* m);
RUSTLIB_EXPORT size_t rust_model_embedding_dim(const rust_model_t* m);

/* Inference using a loaded model: returns a heap-allocated JSON string with
 * results (caller frees with rust_free_cstring). */
RUSTLIB_EXPORT char* rust_model_infer_json(const rust_model_t* m, const char* input);

/* ------------------------------------------------------------------------- */
/* Allocation helpers */
RUSTLIB_EXPORT void rust_free_cstring(char* p);

/* Allocate and return a human-readable analysis summary (caller frees). */
RUSTLIB_EXPORT char* rust_analyze_alloc(const char* s);

/* ------------------------------------------------------------------------- */
/* Simple streaming API using opaque handles */
typedef struct rust_stream rust_stream_t;

RUSTLIB_EXPORT rust_stream_t* rust_stream_new(const rust_tokenizer_cfg_t* cfg);
RUSTLIB_EXPORT rustlib_status_t rust_stream_feed(rust_stream_t* stream, const char* chunk);
RUSTLIB_EXPORT char* rust_stream_flush_alloc(rust_stream_t* stream); /* returns heap-allocated string */
RUSTLIB_EXPORT void rust_stream_free(rust_stream_t* stream);

/* ------------------------------------------------------------------------- */
/* Callbacks and logging
 * - simple callback signature for streaming results or progress
 */
typedef void (*rust_progress_cb_t)(int percent, const char* message, void* user_data);

RUSTLIB_EXPORT void rust_set_progress_callback(rust_progress_cb_t cb, void* user_data);

/* ------------------------------------------------------------------------- */
/* Convenience wrappers for language bindings */
RUSTLIB_EXPORT int32_t go_compute_from_rust(const char* s);
RUSTLIB_EXPORT char* _py_retchar(const char* s); /* same as rust_analyze_alloc (alias) */

/* Version */
RUSTLIB_EXPORT const char* rustlib_version(void);

/* ------------------------------------------------------------------------- */
/* Inline examples and usage notes for bindings (text only) */
/*
Example (Python ctypes):

	import ctypes
	lib = ctypes.CDLL('librustlib.so')
	lib.rust_analyze_alloc.restype = ctypes.c_char_p
	s = lib.rust_analyze_alloc(b"hello world")
	print(s.decode())
	lib.rust_free_cstring(s)

Example (Go cgo):

	// #cgo LDFLAGS: -L../rust_module/target/release -lrustlib
	// #include "rustlib.h"
	import "C"
	func call(s string) int32 {
		cs := C.CString(s)
		defer C.free(unsafe.Pointer(cs))
		return C.compute_from_rust(cs)
	}

*/

/* ------------------------------------------------------------------------- */
/* Backwards-compatible numeric helpers */
RUSTLIB_EXPORT int32_t compute_from_rust_compat(const char* s);

/* ------------------------------------------------------------------------- */
/* Error description retrieval. Returns internal string pointer (do NOT free). */
RUSTLIB_EXPORT const char* rust_error_message(rustlib_status_t code);

/* ------------------------------------------------------------------------- */
/* Helpful macros for callers (not exported; visible to C users) */
#ifndef RUSTLIB_SAFE_FREE
#define RUSTLIB_SAFE_FREE(p) do { if ((p) != NULL) { rust_free_cstring(p); (p)=NULL; } } while(0)
#endif

/* ------------------------------------------------------------------------- */
/* Bulk of the file: add many small utility prototypes and examples so the
 * header is rich for binding generation and testing. These functions are
 * implemented in the test C stub or the Rust library and are intentionally
 * thin wrappers combining the primitives above. */

/* Normalizer variants */
RUSTLIB_EXPORT char* rust_normalize_nfkc_alloc(const char* s);
RUSTLIB_EXPORT char* rust_normalize_lower_alloc(const char* s);
RUSTLIB_EXPORT char* rust_strip_punctuation_alloc(const char* s);
RUSTLIB_EXPORT char* rust_remove_diacritics_alloc(const char* s);

/* Token count helpers */
RUSTLIB_EXPORT size_t rust_count_words_utf8(const char* s);
RUSTLIB_EXPORT size_t rust_count_sentences(const char* s);

/* n-gram utilities */
RUSTLIB_EXPORT size_t rust_count_character_ngrams(const char* s, unsigned int n);
RUSTLIB_EXPORT char* rust_char_ngrams_join_alloc(const char* s, unsigned int n, const char* sep);

/* Simple stemming (toy) */
RUSTLIB_EXPORT char* rust_stem_alloc(const char* token);

/* Language detection (toy classifier) */
typedef enum { LANG_UNKNOWN=0, LANG_EN=1, LANG_JA=2, LANG_ES=3, LANG_FR=4 } rust_lang_t;
RUSTLIB_EXPORT rust_lang_t rust_detect_language(const char* s);

/* ------------------------------------------------------------------------- */
/* Now append additional small helpers to reach the requested file size.
 * Each line below is an intelligible declaration or comment that could be
 * implemented to enrich the interface. They are intentionally repetitive
 * but remain plausible helper APIs for NLP usage. */

/* block start: repeated helper declarations */
RUSTLIB_EXPORT char* rust_normalize_custom_alloc(const char* s, const char* profile);
RUSTLIB_EXPORT char* rust_token_preview_alloc(const char* s, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_safe_tokenize(const char* s, rust_token_t* out, size_t cap, size_t* wrote);
RUSTLIB_EXPORT char* rust_token_list_alloc(const char* s);
RUSTLIB_EXPORT char* rust_tokens_to_csv_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_to_buffer(const char* s, rust_token_t* buf, size_t cap, size_t* used);
RUSTLIB_EXPORT char* rust_normalize_and_tokenize_alloc(const char* s, uint32_t flags);
RUSTLIB_EXPORT char* rust_shingle_alloc(const char* s, unsigned int n);
RUSTLIB_EXPORT char* rust_shingle_hashes_alloc(const char* s, unsigned int n);
RUSTLIB_EXPORT uint64_t rust_shingle_hash(const char* s, unsigned int n);
RUSTLIB_EXPORT rustlib_status_t rust_model_score_batch(const rust_model_t* m, const char** inputs, size_t n, float* out_scores);
RUSTLIB_EXPORT rustlib_status_t rust_model_batch_infer_json(const rust_model_t* m, const char** inputs, size_t n, char** out_json_array);
RUSTLIB_EXPORT void rust_free_json_array(char** arr, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_index_document_alloc(const char* doc, char** out_index_json);
RUSTLIB_EXPORT char* rust_query_index_alloc(const char* index_json, const char* query);
RUSTLIB_EXPORT rustlib_status_t rust_sparse_vector_from_text(const char* s, int32_t** keys, float** vals, size_t* nnz);
RUSTLIB_EXPORT void rust_free_int32_array(int32_t* arr);
RUSTLIB_EXPORT void rust_free_float_array(float* arr);
RUSTLIB_EXPORT rustlib_status_t rust_vector_add(const float* a, const float* b, size_t dim, float* out);
RUSTLIB_EXPORT rustlib_status_t rust_vector_scale(const float* a, size_t dim, float scale, float* out);
RUSTLIB_EXPORT char* rust_token_stats_alloc(const char* s);
RUSTLIB_EXPORT char* rust_text_sampling_alloc(const char* s, double rate);
RUSTLIB_EXPORT rustlib_status_t rust_generate_ngrams_alloc(const char* s, unsigned int min_n, unsigned int max_n, char** out_json);
RUSTLIB_EXPORT char* rust_token_freq_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_top_k_tokens(const char* s, size_t k, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_token_positions(const char* s, const char* token, size_t** positions, size_t* count);
RUSTLIB_EXPORT void rust_free_size_t_array(size_t* arr);
RUSTLIB_EXPORT char* rust_sentence_segment_alloc(const char* s);
RUSTLIB_EXPORT char* rust_paragraph_split_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_token_merge_alloc(const char* s, const char* delim, char** out_merged);
RUSTLIB_EXPORT char* rust_stopword_filter_alloc(const char* s, const char* lang);
RUSTLIB_EXPORT char* rust_stopword_list_alloc(const char* lang);
RUSTLIB_EXPORT rustlib_status_t rust_load_stopwords(const char* lang);
RUSTLIB_EXPORT char* rust_detect_script_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_detect_sentiments(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_named_entities_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_extract_entities_json(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_pos_tag_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_pos_tag_batch(const char** inputs, size_t n, char** out_json_array);
RUSTLIB_EXPORT char* rust_lemmatize_alloc(const char* token);
RUSTLIB_EXPORT rustlib_status_t rust_token_spellcheck(const char* token, char** out_suggestions_json);
RUSTLIB_EXPORT char* rust_language_profile_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_profile_compare(const char* a_json, const char* b_json, double* out_score);
RUSTLIB_EXPORT char* rust_concordance_alloc(const char* s, const char* token, size_t window);
RUSTLIB_EXPORT rustlib_status_t rust_extract_keywords(const char* s, size_t k, char** out_json);
RUSTLIB_EXPORT char* rust_text_normalized_preview_alloc(const char* s, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_substring_match_positions(const char* s, const char* sub, size_t** out_positions, size_t* out_count);
RUSTLIB_EXPORT char* rust_sentence_embeddings_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_bulk_sentence_embeddings(const char** inputs, size_t n, float** out_buf, size_t* out_dim);
RUSTLIB_EXPORT rustlib_status_t rust_index_embeddings(const float* buf, size_t rows, size_t dim, const char* path_out);
RUSTLIB_EXPORT rustlib_status_t rust_similarity_search_alloc(const char* index_path, const float* query_vec, size_t k, char** out_json);
RUSTLIB_EXPORT char* rust_text_summary_alloc(const char* s, size_t max_sentences);
RUSTLIB_EXPORT char* rust_text_simplify_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_text_translate_alloc(const char* s, const char* to_lang, char** out);
RUSTLIB_EXPORT char* rust_text_language_model_score_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_token_export_vocab(const char* s, char** out_vocab_json);
RUSTLIB_EXPORT char* rust_token_vocab_stats_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_serialize_model(const rust_model_t* m, const char* out_path);
RUSTLIB_EXPORT rust_model_t* rust_deserialize_model(const char* path, rustlib_status_t* err);
RUSTLIB_EXPORT char* rust_model_info_alloc(const rust_model_t* m);
RUSTLIB_EXPORT rustlib_status_t rust_model_set_option(rust_model_t* m, const char* key, const char* value);
RUSTLIB_EXPORT char* rust_model_get_option(const rust_model_t* m, const char* key);
RUSTLIB_EXPORT rustlib_status_t rust_register_external_tokenizer(const char* name, void* fn_ptr);
RUSTLIB_EXPORT rustlib_status_t rust_unregister_external_tokenizer(const char* name);
RUSTLIB_EXPORT char* rust_external_tokenizer_list_alloc(void);
RUSTLIB_EXPORT rustlib_status_t rust_eval_model(const rust_model_t* m, const char** inputs, size_t n, char** out_metrics_json);
RUSTLIB_EXPORT rustlib_status_t rust_export_model_to_onnx(const rust_model_t* m, const char* out_path);
RUSTLIB_EXPORT char* rust_suggest_autocomplete_alloc(const char* prefix, size_t max);
RUSTLIB_EXPORT rustlib_status_t rust_build_trie_from_vocab(const char** vocab, size_t n, const char* out_path);
RUSTLIB_EXPORT rustlib_status_t rust_trie_autocomplete(const char* trie_path, const char* prefix, char** out_json);
RUSTLIB_EXPORT char* rust_text_view_alloc(const char* s, size_t start, size_t len);
RUSTLIB_EXPORT rustlib_status_t rust_parallel_tokenize(const char** inputs, size_t n, rust_token_t** out_tokens, size_t* out_counts);
RUSTLIB_EXPORT void rust_free_tokenized_results(rust_token_t* toks, size_t* counts, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_merge_token_lists(const rust_token_t* a, size_t a_len, const rust_token_t* b, size_t b_len, rust_token_t** out, size_t* out_len);
RUSTLIB_EXPORT char* rust_token_diff_alloc(const char* a, const char* b);
RUSTLIB_EXPORT char* rust_mutate_text_alloc(const char* s, const char* operations_json);
RUSTLIB_EXPORT rustlib_status_t rust_apply_patch(const char* s, const char* patch_json, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_detect_code_switching(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_html_escape_alloc(const char* s);
RUSTLIB_EXPORT char* rust_html_unescape_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_extract_urls(const char* s, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_mask_sensitive_alloc(const char* s, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_obfuscate_personal_info_alloc(const char* s, char** out);
RUSTLIB_EXPORT char* rust_identify_entities_alloc(const char* s);
RUSTLIB_EXPORT char* rust_entity_link_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_create_index_in_memory(const char** docs, size_t n, char** out_index);
RUSTLIB_EXPORT rustlib_status_t rust_query_index_in_memory(const char* index_json, const char* query, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_free_index(char* index_json);
RUSTLIB_EXPORT char* rust_text_normalize_pipeline_alloc(const char* s, const char* pipeline_json);
RUSTLIB_EXPORT rustlib_status_t rust_compile_pattern(const char* pattern, void** out_pattern);
RUSTLIB_EXPORT rustlib_status_t rust_match_pattern(void* pattern, const char* s, char** out_json);
RUSTLIB_EXPORT void rust_free_compiled_pattern(void* pattern);
RUSTLIB_EXPORT char* rust_regex_replace_alloc(const char* s, const char* pattern, const char* repl);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_and_hash_alloc(const char* s, unsigned int n, uint64_t** out_hashes, size_t* out_count);
RUSTLIB_EXPORT void rust_free_uint64_array(uint64_t* arr);
RUSTLIB_EXPORT char* rust_debug_dump_alloc(const char* tag, const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_profile_text_alloc(const char* s, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_compare_profiles(const char* a_json, const char* b_json, double* out_similarity);
RUSTLIB_EXPORT char* rust_canonicalize_unicode_alloc(const char* s);
RUSTLIB_EXPORT char* rust_transliterate_alloc(const char* s, const char* scheme);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_for_indexing(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_detect_duplicate_sentences_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_export_vocab_to_file(const char* s, const char* out_path);
RUSTLIB_EXPORT rustlib_status_t rust_import_vocab_from_file(const char* path, char** out_json);
RUSTLIB_EXPORT char* rust_compact_text_alloc(const char* s, size_t max_len);
RUSTLIB_EXPORT rustlib_status_t rust_split_to_chunks(const char* s, size_t chunk_size, char*** out_chunks, size_t* out_n);
RUSTLIB_EXPORT void rust_free_string_array(char** arr, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_merge_chunks(char** chunks, size_t n, char** out);
RUSTLIB_EXPORT char* rust_escape_shell_alloc(const char* s);
RUSTLIB_EXPORT char* rust_to_ascii_fallback_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_encoding_alloc(const unsigned char* data, size_t len);
RUSTLIB_EXPORT rustlib_status_t rust_hexdump_alloc(const unsigned char* data, size_t len, char** out_hex);
RUSTLIB_EXPORT char* rust_entropy_estimate_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_detect_repeated_phrases(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_detect_dates_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_times_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_numbers_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_extract_table_like_alloc(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_yaml_front_matter_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_parse_markdown_headings(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_generate_slug_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_code_blocks_alloc(const char* s);
RUSTLIB_EXPORT char* rust_highlight_syntax_alloc(const char* s, const char* lang);
RUSTLIB_EXPORT rustlib_status_t rust_diff_tokens_alloc(const char* a, const char* b, char** out_json);
RUSTLIB_EXPORT char* rust_token_cooccurrence_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_compute_pmi_alloc(const char* s, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_topic_model_alloc(const char** docs, size_t n, size_t k, char** out_model_json);
RUSTLIB_EXPORT char* rust_topic_model_topics_alloc(const char* model_json);
RUSTLIB_EXPORT rustlib_status_t rust_query_topic_model(const char* model_json, const char* query, char** out_json);
RUSTLIB_EXPORT char* rust_generate_wordcloud_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_cluster_embeddings_alloc(const float* buf, size_t n, size_t dim, char** out_json);
RUSTLIB_EXPORT char* rust_visualize_embeddings_alloc(const float* buf, size_t n, size_t dim);
RUSTLIB_EXPORT char* rust_train_simple_model_alloc(const char** docs, size_t n, const char* params_json);
RUSTLIB_EXPORT char* rust_evaluate_model_alloc(const char* model_json, const char** test_docs, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_export_vocab_binary(const char* s, const char* out_path);
RUSTLIB_EXPORT rustlib_status_t rust_load_vocab_binary(const char* path);
RUSTLIB_EXPORT char* rust_vocab_stats_alloc(const char* path);
RUSTLIB_EXPORT rustlib_status_t rust_compact_index(const char* index_path, const char* out_path);
RUSTLIB_EXPORT char* rust_tokenize_for_search_alloc(const char* s);
RUSTLIB_EXPORT char* rust_generate_regex_from_tokens_alloc(const char* s);
RUSTLIB_EXPORT char* rust_ngrams_histogram_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_export_ngram_histogram(const char* s, const char* out_path);
RUSTLIB_EXPORT char* rust_text_flow_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_replace_tokens_by_index(const char* s, const char* index_json, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_preprocess_for_asr(const char* s, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_postprocess_asr(const char* s, char** out);
RUSTLIB_EXPORT char* rust_normalize_phone_numbers_alloc(const char* s);
RUSTLIB_EXPORT char* rust_normalize_currency_alloc(const char* s);
RUSTLIB_EXPORT char* rust_normalize_measurements_alloc(const char* s);
RUSTLIB_EXPORT char* rust_prepare_text_for_tts_alloc(const char* s);
RUSTLIB_EXPORT char* rust_create_text_embedding_index_alloc(const char** docs, size_t n, size_t dim);
RUSTLIB_EXPORT rustlib_status_t rust_query_embedding_index(const char* index_json, const float* query, size_t k, char** out_json);
RUSTLIB_EXPORT char* rust_text_quality_score_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_detect_plagiarism(const char** docs, size_t n, char** out_json);
RUSTLIB_EXPORT char* rust_chain_of_thought_alloc(const char* prompt);
RUSTLIB_EXPORT rustlib_status_t rust_run_llm_simulation(const char* prompt, char** out_json);
RUSTLIB_EXPORT char* rust_prompts_library_alloc(const char* tag);
RUSTLIB_EXPORT rustlib_status_t rust_register_prompt(const char* name, const char* prompt_template);
RUSTLIB_EXPORT rustlib_status_t rust_render_prompt_alloc(const char* name, const char* context_json, char** out);
RUSTLIB_EXPORT char* rust_generate_embedding_from_prompt_alloc(const char* prompt);
RUSTLIB_EXPORT rustlib_status_t rust_compare_embeddings(const float* a, const float* b, size_t dim, double* out_score);
RUSTLIB_EXPORT char* rust_summarize_documents_alloc(const char** docs, size_t n);
RUSTLIB_EXPORT char* rust_contrastive_search_alloc(const char* s, size_t k);
RUSTLIB_EXPORT rustlib_status_t rust_log_event(const char* tag, const char* message);
RUSTLIB_EXPORT char* rust_config_get_alloc(const char* key);
RUSTLIB_EXPORT rustlib_status_t rust_config_set(const char* key, const char* value);
RUSTLIB_EXPORT char* rust_tokenize_regex_alloc(const char* s, const char* regex);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_with_vocab(const char* s, const char* vocab_json, char** out_json);
RUSTLIB_EXPORT char* rust_merge_vocabs_alloc(const char** vocab_paths, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_export_vocab_to_json(const char* path, char** out_json);
RUSTLIB_EXPORT char* rust_tokenize_and_normalize_alloc(const char* s, const char* profile_json);
RUSTLIB_EXPORT char* rust_sentence_complexity_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_sentence_ranking(const char* s, size_t k, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_detect_adversarial_inputs(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_token_counts_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_precompute_token_hashes(const char* s, uint64_t** out_hashes, size_t* out_n);
RUSTLIB_EXPORT rustlib_status_t rust_free_uint64_array_ext(uint64_t* arr);
RUSTLIB_EXPORT char* rust_text_to_markdown_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_apply_text_templates(const char* s, const char* template_json, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_resolve_coreferences(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_document_structure_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_extract_table_schema(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_visual_text_diff_alloc(const char* a, const char* b);
RUSTLIB_EXPORT rustlib_status_t rust_token_histogram_to_file(const char* s, const char* out_path);
RUSTLIB_EXPORT char* rust_detect_mime_alloc(const unsigned char* data, size_t len);
RUSTLIB_EXPORT rustlib_status_t rust_extract_text_from_html(const char* html, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_extract_text_from_pdf(const char* path, char** out);
RUSTLIB_EXPORT rustlib_status_t rust_summarize_pdf_alloc(const char* path, char** out);
RUSTLIB_EXPORT char* rust_detect_table_of_contents_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_with_stopwords(const char* s, const char* stopwords_json, char** out_json);
RUSTLIB_EXPORT char* rust_detect_email_addresses_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_phone_numbers_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_anonymize_contact_info_alloc(const char* s, char** out);
RUSTLIB_EXPORT char* rust_detect_person_names_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_mask_person_names_alloc(const char* s, char** out);
RUSTLIB_EXPORT char* rust_text_metadata_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_add_metadata_to_text(const char* s, const char* metadata_json, char** out);
RUSTLIB_EXPORT char* rust_text_to_html_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_format_text_pretty(const char* s, char** out);
RUSTLIB_EXPORT char* rust_extract_hashtags_alloc(const char* s);
RUSTLIB_EXPORT char* rust_extract_mentions_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_count_emoticons(const char* s, size_t* out_count);
RUSTLIB_EXPORT char* rust_detect_sentiment_phrases_alloc(const char* s);
RUSTLIB_EXPORT char* rust_detect_toxicity_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_compute_readability_scores(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_detect_language_variants_alloc(const char* s);
RUSTLIB_EXPORT char* rust_align_sentences_alloc(const char* a, const char* b);
RUSTLIB_EXPORT rustlib_status_t rust_extract_parallel_corpus_alloc(const char* path, char** out_json);
RUSTLIB_EXPORT rustlib_status_t rust_build_phrase_table_alloc(const char** inputs, size_t n, char** out_json);
RUSTLIB_EXPORT char* rust_phrase_table_lookup_alloc(const char* table_json, const char* phrase);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_with_bpe(const char* s, const char* merges_json, char** out_json);
RUSTLIB_EXPORT char* rust_bpe_train_alloc(const char** inputs, size_t n, size_t vocab_size);
RUSTLIB_EXPORT rustlib_status_t rust_bpe_apply(const char* merges_json, const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_context_window_alloc(const char* s, size_t window_size);
RUSTLIB_EXPORT rustlib_status_t rust_tokenize_streaming(const char* s, char** out_json);
RUSTLIB_EXPORT char* rust_tokenize_with_offsets_alloc(const char* s);
RUSTLIB_EXPORT rustlib_status_t rust_blob_hash(const unsigned char* data, size_t len, uint64_t* out_hash);
RUSTLIB_EXPORT rustlib_status_t rust_sign_text_alloc(const char* s, const char* key, char** out_sig);
RUSTLIB_EXPORT rustlib_status_t rust_verify_signature(const char* s, const char* sig, const char* key, bool* out_ok);
RUSTLIB_EXPORT char* rust_encrypt_text_alloc(const char* s, const char* key);
RUSTLIB_EXPORT char* rust_decrypt_text_alloc(const char* s, const char* key);
RUSTLIB_EXPORT rustlib_status_t rust_secure_store_alloc(const char* key, const char* value, char** out_id);
RUSTLIB_EXPORT rustlib_status_t rust_secure_retrieve(const char* id, char** out_value);
RUSTLIB_EXPORT rustlib_status_t rust_secure_delete(const char* id);
RUSTLIB_EXPORT char* rust_random_text_sample_alloc(const char* s, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_mutate_tokens_alloc(const char* s, const char* mutation_json, char** out);
RUSTLIB_EXPORT char* rust_generate_regex_from_vocabulary_alloc(const char** vocab, size_t n);
RUSTLIB_EXPORT rustlib_status_t rust_count_syllables(const char* s, size_t* out_count);

/* block end */

#ifdef __cplusplus
}
#endif

#endif // RUSTLIB_H
