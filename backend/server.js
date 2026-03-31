const express    = require('express');
const cors       = require('cors');
const helmet     = require('helmet');
const morgan     = require('morgan');
require('dotenv').config();

const simulateRouter = require('./routes/simulate');

const app  = express();
const PORT = process.env.PORT || 3001;

// ─────────────────────────────────────────
//  Middleware
// ─────────────────────────────────────────
app.use(helmet());
app.use(cors({
  origin: process.env.FRONTEND_URL || 'http://localhost:5173',
  methods: ['GET', 'POST'],
}));
app.use(express.json({ limit: '1mb' }));
app.use(morgan('dev'));

// ─────────────────────────────────────────
//  Routes
// ─────────────────────────────────────────
app.use('/api/simulate', simulateRouter);

app.get('/api/health', (_req, res) => {
  const { isNative } = require('./physicsAddon');
  res.json({
    status:  'ok',
    engine:  isNative() ? 'C++ N-API' : 'JS mock',
    version: '1.0.0',
  });
});

app.use((_req, res) => res.status(404).json({ error: 'Not found' }));

app.use((err, _req, res, _next) => {
  console.error('[server error]', err);
  res.status(500).json({ error: err.message || 'Internal server error' });
});

// ─────────────────────────────────────────
//  Start
// ─────────────────────────────────────────
app.listen(PORT, () => {
  console.log(`\n🔬 PhysicaEngine backend running on http://localhost:${PORT}`);
  console.log(`   Engine: ${require('./physicsAddon').isNative() ? '✓ C++ N-API' : '⚠ JS mock (run engine build)'}\n`);
});

module.exports = app;