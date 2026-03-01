/**
 * observer_model.js
 * Lightweight observer-relative interpretation utilities.
 * Exports a small API so the model can be reused from other modules or tests.
 */

/**
 * Interpret how a list of observers perceive a single entity.
 * @param {string} entity - canonical entity identifier or name
 * @param {Array<{name:string, sees:string|null}>} observers
 * @returns {Array<{observer:string, perceived:string|null}>}
 */
function interpretObservers(entity, observers) {
  if (!entity || !Array.isArray(observers)) return [];
  return observers.map(obs => {
    const perceived = obs.sees === entity ? entity : (obs.sees || null);
    return { observer: obs.name, perceived };
  });
}

// Default demo when run directly via Node.js
if (typeof require !== 'undefined' && require.main === module) {
  const defaultObservers = [
    { name: 'A', sees: 'the man' },
    { name: 'B', sees: 'the voice' },
    { name: 'C', sees: null }
  ];
  const results = interpretObservers('the man', defaultObservers);
  for (const r of results) {
    if (r.perceived === null) console.log(`Observer ${r.observer} does not perceive the entity.`);
    else if (r.perceived === 'the man') console.log(`Observer ${r.observer} sees entity as itself.`);
    else console.log(`Observer ${r.observer} sees entity as: ${r.perceived}`);
  }
}

module.exports = { interpretObservers };
