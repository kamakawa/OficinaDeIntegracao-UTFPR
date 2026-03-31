const express  = require('express');
const router   = express.Router();
const ctrl     = require('../controllers/physicsController');

// POST /api/simulate/projectile
router.post('/projectile', ctrl.projectile);

// POST /api/simulate/waves
router.post('/waves', ctrl.waves);

// POST /api/simulate/electric-field
router.post('/electric-field', ctrl.electricField);

// POST /api/simulate/rlc
router.post('/rlc', ctrl.rlc);

// POST /api/simulate/shm
router.post('/shm', ctrl.shm);

module.exports = router;