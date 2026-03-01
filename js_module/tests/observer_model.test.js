const { interpretObservers } = require('../observer_model');

test('interpretObservers returns expected perceptions', () => {
  const observers = [
    { name: 'A', sees: 'the man' },
    { name: 'B', sees: 'the voice' },
    { name: 'C', sees: null }
  ];
  const res = interpretObservers('the man', observers);
  expect(res).toHaveLength(3);
  expect(res[0]).toEqual({ observer: 'A', perceived: 'the man' });
  expect(res[1]).toEqual({ observer: 'B', perceived: 'the voice' });
  expect(res[2]).toEqual({ observer: 'C', perceived: null });
});
