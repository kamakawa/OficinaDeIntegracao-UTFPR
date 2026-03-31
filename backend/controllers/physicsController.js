const physics = require('../physicsAddon');

// ─────────────────────────────────────────
//  Validation helper
// ─────────────────────────────────────────
function requireFields(body, fields) {
  for (const f of fields) {
    if (body[f] === undefined) return `Missing required field: ${f}`;
  }
  return null;
}

// ─────────────────────────────────────────
//  POST /api/simulate/projectile
//  Body: { vx0, vy0, mass, dragCoef, area, g, dt, tEnd }
// ─────────────────────────────────────────
exports.projectile = async (req, res, next) => {
  try {
    const params = {
      x0:       req.body.x0       ?? 0,
      y0:       req.body.y0       ?? 0,
      vx0:      req.body.vx0      ?? 50,
      vy0:      req.body.vy0      ?? 50,
      mass:     req.body.mass     ?? 1.0,
      dragCoef: req.body.dragCoef ?? 0.47,
      area:     req.body.area     ?? 0.01,
      rhoAir:   req.body.rhoAir   ?? 1.225,
      g:        req.body.g        ?? 9.81,
      dt:       req.body.dt       ?? 0.01,
      tEnd:     req.body.tEnd     ?? 30,
    };
    const data = await physics.simulateProjectile(params);
    res.json({ ok: true, module: 'projectile', count: data.length, data });
  } catch (err) { next(err); }
};

// ─────────────────────────────────────────
//  POST /api/simulate/waves
//  Body: { sources: [{amplitude,frequency,phase,x0,y0,waveSpeed}], t, xMin, xMax, yMin, yMax, nx, ny }
// ─────────────────────────────────────────
exports.waves = async (req, res, next) => {
  try {
    const params = {
      sources:  req.body.sources  ?? [{ amplitude:1, frequency:1, phase:0, x0:0, y0:0, waveSpeed:1 }],
      t:        req.body.t        ?? 0,
      xMin:     req.body.xMin     ?? -10,
      xMax:     req.body.xMax     ??  10,
      yMin:     req.body.yMin     ?? -10,
      yMax:     req.body.yMax     ??  10,
      nx:       req.body.nx       ?? 80,
      ny:       req.body.ny       ?? 80,
    };
    const data = await physics.simulateWaves(params);
    res.json({ ok: true, module: 'waves', count: data.length, data });
  } catch (err) { next(err); }
};

// ─────────────────────────────────────────
//  POST /api/simulate/electric-field
//  Body: { charges: [{q,x,y}], xMin, xMax, yMin, yMax, nx, ny }
// ─────────────────────────────────────────
exports.electricField = async (req, res, next) => {
  try {
    const params = {
      charges: req.body.charges ?? [{ q: 1e-9, x: 0, y: 0 }],
      xMin:    req.body.xMin    ?? -5,
      xMax:    req.body.xMax    ??  5,
      yMin:    req.body.yMin    ?? -5,
      yMax:    req.body.yMax    ??  5,
      nx:      req.body.nx      ?? 40,
      ny:      req.body.ny      ?? 40,
    };
    const data = await physics.simulateElectricField(params);
    res.json({ ok: true, module: 'electric-field', count: data.length, data });
  } catch (err) { next(err); }
};

// ─────────────────────────────────────────
//  POST /api/simulate/rlc
// ─────────────────────────────────────────
exports.rlc = async (req, res, next) => {
  try {
    const params = {
      V0:   req.body.V0   ?? 10,
      R:    req.body.R    ?? 10,
      L:    req.body.L    ?? 0.1,
      C:    req.body.C    ?? 1e-4,
      dt:   req.body.dt   ?? 1e-5,
      tEnd: req.body.tEnd ?? 0.05,
    };
    const data = await physics.simulateRLC(params);
    res.json({ ok: true, module: 'rlc', count: data.length, data });
  } catch (err) { next(err); }
};

// ─────────────────────────────────────────
//  POST /api/simulate/shm  (pure JS, lightweight)
// ─────────────────────────────────────────
exports.shm = (req, res, next) => {
  try {
    const { k = 10, m = 1, x0 = 1, v0 = 0, dt = 0.01, tEnd = 10 } = req.body;
    const omega = Math.sqrt(k / m);
    const data  = [];
    for (let t = 0; t <= tEnd; t += dt) {
      const x = x0 * Math.cos(omega * t) + (v0 / omega) * Math.sin(omega * t);
      const v = -x0 * omega * Math.sin(omega * t) + v0 * Math.cos(omega * t);
      data.push({ t: +t.toFixed(4), x, v, ke: 0.5 * m * v * v, pe: 0.5 * k * x * x });
    }
    res.json({ ok: true, module: 'shm', count: data.length, data });
  } catch (err) { next(err); }
};