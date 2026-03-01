// Lightweight test runner using Node built-in `assert` so tests can run
// in older Node environments without Jest.
const assert = require('assert');

const { interpretObservers } = require('./observer_model');
const { Observer, Entity, interpretAll } = require('./observer_model_highperf');

async function test_interpretObservers() {
  const observers = [
    { name: 'A', sees: 'the man' },
    { name: 'B', sees: 'the voice' },
    { name: 'C', sees: null }
  ];
  const res = interpretObservers('the man', observers);
  assert.strictEqual(res.length, 3);
  assert.deepStrictEqual(res[0], { observer: 'A', perceived: 'the man' });
  assert.deepStrictEqual(res[1], { observer: 'B', perceived: 'the voice' });
  assert.deepStrictEqual(res[2], { observer: 'C', perceived: null });
}

async function test_interpretAll() {
  const observers = [
    new Observer('A', e => (e.name === 'x' ? 'x' : null)),
    new Observer('B', async e => (e.state === 'active' ? 'active' : null))
  ];
  const entities = [new Entity(1, 'x', 'inactive'), new Entity(2, 'y', 'active')];
  const out = await interpretAll(observers, entities);
  assert.strictEqual(out.length, 2);
  assert.strictEqual(out[0].results[0].view, 'x');
  assert.strictEqual(out[0].results[1].view, null);
  assert.strictEqual(out[1].results[0].view, null);
  assert.strictEqual(out[1].results[1].view, 'active');
}

(async () => {
  try {
    await test_interpretObservers();
    console.log('✓ interpretObservers passed');
    await test_interpretAll();
    console.log('✓ interpretAll (highperf) passed');
    console.log('\nAll node-based tests passed');
    process.exit(0);
  } catch (err) {
    console.error('Test failed:', err && err.stack ? err.stack : err);
    process.exit(2);
  }
})();
