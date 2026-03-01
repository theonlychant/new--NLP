/**
 * main.js — small entrypoint demonstrating the JS module usage.
 * Shows how to use the observer models and optionally call an external
 * Python NLP service via HTTP (simple example).
 */

const http = require('http');
const { interpretObservers } = require('./observer_model');

function demoLocal() {
	const observers = [
		{ name: 'A', sees: 'the man' },
		{ name: 'B', sees: 'the voice' },
		{ name: 'C', sees: null }
	];
	const results = interpretObservers('the man', observers);
	console.log('Local interpretation demo:');
	results.forEach(r => console.log(` - ${r.observer}: ${r.perceived}`));
}

async function callPythonService(host = 'localhost', port = 8000, text = 'hello world') {
	const payload = JSON.stringify({ text });
	const opts = { hostname: host, port, path: '/analyze', method: 'POST', headers: { 'Content-Type': 'application/json', 'Content-Length': Buffer.byteLength(payload) } };
	return new Promise((resolve, reject) => {
		const req = http.request(opts, res => {
			let data = '';
			res.on('data', c => data += c);
			res.on('end', () => resolve({ status: res.statusCode, body: data }));
		});
		req.on('error', reject);
		req.write(payload);
		req.end();
	});
}

if (require.main === module) {
	demoLocal();
	// Example: attempt calling a local Python service (non-blocking)
	callPythonService('localhost', 8000, 'hello from js').then(r => console.log('Python service response:', r)).catch(() => console.log('Python service not available'));
}

module.exports = { demoLocal, callPythonService };
