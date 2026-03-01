const { Observer, Entity, interpretAll } = require('../observer_model_highperf');

test('interpretAll produces views for observers', async () => {
  const observers = [
    new Observer('A', e => (e.name === 'x' ? 'x' : null)),
    new Observer('B', async e => { return e.state === 'active' ? 'active' : null })
  ];
  const entities = [new Entity(1, 'x', 'inactive'), new Entity(2, 'y', 'active')];
  const out = await interpretAll(observers, entities);
  expect(out).toHaveLength(2);
  // Observer A sees only entity 1
  expect(out[0].results[0].view).toBe('x');
  expect(out[0].results[1].view).toBeNull();
  // Observer B sees only entity 2
  expect(out[1].results[0].view).toBeNull();
  expect(out[1].results[1].view).toBe('active');
});
