/**
 * physicsAddon.js
 * Loads the compiled N-API addon and exposes a clean async API.
 * Falls back to a JS mock if the addon hasn't been compiled yet
 * (useful during frontend-only development).
 */
const path = require('path');

let addon;
try {
  addon = require(path.join(__dirname, '..', 'engine', 'build', 'Release', 'physics_engine.node'));
  console.log('[PhysicaEngine] C++ addon loaded ✓');
} catch (err) {
  console.warn('[PhysicaEngine] C++ addon not found — using JS mock');
  console.warn('[PhysicaEngine] Run: cd engine && npm run build');
  addon = null;
}

// ─────────────────────────────────────────
//  JS fallback mocks (for dev without build)
// ─────────────────────────────────────────
function mockProjectile(params) {
  const { vx0 = 50, vy0 = 50, g = 9.81, dt = 0.05 } = params;
  const pts = [];
  let x = 0, y = 0, vx = vx0, vy = vy0, t = 0;
  while (y >= 0 && t < 30) {
    pts.push({ t, x, y, vx, vy, speed: Math.hypot(vx, vy), ke: 0.5 * Math.hypot(vx, vy) ** 2 });
    vy -= g * dt;
    x  += vx * dt;
    y  += vy * dt;
    t  += dt;
  }
  return Promise.resolve(pts);
}

function mockWaves(params) {
  const { nx = 40, ny = 40, xMin = -10, xMax = 10, yMin = -10, yMax = 10, t = 0 } = params;
  const pts = [];
  for (let j = 0; j < ny; j++) {
    for (let i = 0; i < nx; i++) {
      const x = xMin + (i / (nx - 1)) * (xMax - xMin);
      const y = yMin + (j / (ny - 1)) * (yMax - yMin);
      pts.push({ x, y, amp: Math.sin(Math.hypot(x, y) - t * 5) });
    }
  }
  return Promise.resolve(pts);
}

function mockElectricField(params) {
  const { nx = 20, ny = 20, xMin = -5, xMax = 5, yMin = -5, yMax = 5 } = params;
  const pts = [];
  for (let j = 0; j < ny; j++) {
    for (let i = 0; i < nx; i++) {
      const x = xMin + (i / (nx - 1)) * (xMax - xMin);
      const y = yMin + (j / (ny - 1)) * (yMax - yMin);
      const r2 = x * x + y * y || 0.001;
      pts.push({ x, y, Ex: x / r2, Ey: y / r2, Emag: 1 / Math.sqrt(r2), V: 1 / Math.sqrt(r2) });
    }
  }
  return Promise.resolve(pts);
}

// ─────────────────────────────────────────
//  Public API
// ─────────────────────────────────────────
module.exports = {
  simulateProjectile: (params) =>
    addon ? addon.simulateProjectile(params) : mockProjectile(params),

  simulateWaves: (params) =>
    addon ? addon.simulateWaves(params) : mockWaves(params),

  simulateElectricField: (params) =>
    addon ? addon.simulateElectricField(params) : mockElectricField(params),

  simulateRLC: (params) =>
    addon ? Promise.resolve(addon.simulateRLC(params)) : Promise.resolve([]),

  isNative: () => addon !== null,
};