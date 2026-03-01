// High-Performance Observer-Relative Interpretation Model (JavaScript)
// Provides a small, testable API for mapping observers -> entity views.

class Observer {
  /**
   * @param {string} name
   * @param {(entity:Entity)=>Promise<string|null>|(entity:Entity)=>string|null} perspective
   */
  constructor(name, perspective) {
    this.name = name;
    this.perspective = perspective;
  }
  async interpret(entity) {
    return await this.perspective(entity);
  }
}

class Entity {
  constructor(id, name, state = 'defined', layer = 0) {
    this.id = id;
    this.name = name;
    this.state = state;
    this.layer = layer;
  }
}

/**
 * Interpret a list of entities for each observer. Supports async perspectives.
 * Returns a nested result: { observer, results: [{entity, view}] }
 * @param {Observer[]} observers
 * @param {Entity[]} entities
 */
async function interpretAll(observers, entities) {
  const out = [];
  for (const obs of observers) {
    const results = await Promise.all(entities.map(async ent => ({ entity: ent, view: await obs.interpret(ent) })));
    out.push({ observer: obs.name, results });
  }
  return out;
}

// Exports and demo when run directly
module.exports = { Observer, Entity, interpretAll };

if (typeof require !== 'undefined' && require.main === module) {
  const observers = [
    new Observer('A', e => (e.name === 'the man' ? 'the man' : null)),
    new Observer('B', e => (e.name === 'the voice' ? 'the voice' : null)),
    new Observer('C', e => (e.state === 'Split' ? 'fragmented entity' : null)),
    new Observer('D', async e => { /* simulate async check */ return null; })
  ];

  const entities = [
    new Entity(1, 'the man', 'Defined', 0),
    new Entity(2, 'the voice', 'Defined', 1),
    new Entity(3, 'the man (aspect A)', 'Split', 2),
    new Entity(4, 'the man (aspect B)', 'Split', 2),
  ];

  (async () => {
    const r = await interpretAll(observers, entities);
    for (const row of r) {
      console.log(`Observer ${row.observer}:`);
      for (const item of row.results) {
        if (item.view) console.log(`  sees entity ${item.entity.id} as: ${item.view}`);
        else console.log(`  does not perceive entity ${item.entity.id}`);
      }
    }
  })();
}
