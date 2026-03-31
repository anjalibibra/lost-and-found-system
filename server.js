const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const path = require('path');
const multer = require('multer');

const app = express();
const PORT = process.env.PORT || 3000;
const isVercel = !!process.env.VERCEL;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static('public'));

// Configure multer: Vercel filesystem is read-only except /tmp — use memory + store as data URL in DB
const storage = isVercel
  ? multer.memoryStorage()
  : multer.diskStorage({
      destination: function (req, file, cb) {
        cb(null, 'public/uploads/');
      },
      filename: function (req, file, cb) {
        cb(null, Date.now() + '-' + file.originalname);
      }
    });

const upload = multer({ storage: storage });

// MongoDB connection
const MONGODB_URI = process.env.MONGODB_URI || 'mongodb+srv://admin:admin@cluster0.rmemgma.mongodb.net/lostandfound?retryWrites=true&w=majority&appName=Cluster0';

mongoose.connect(MONGODB_URI)
.then(() => console.log('Connected to MongoDB Atlas'))
.catch(err => console.error('MongoDB connection error:', err));

// Item Schema
const itemSchema = new mongoose.Schema({
  type: {
    type: String,
    required: true,
    enum: ['lost', 'found']
  },
  category: {
    type: String,
    required: true
  },
  location: {
    type: String,
    required: true
  },
  date: {
    type: Date,
    default: Date.now
  },
  image: {
    type: String,
    default: ''
  },
  description: {
    type: String,
    default: ''
  },
  contact: {
    type: String,
    default: ''
  }
});

const Item = mongoose.model('Item', itemSchema);

// Routes

// 1. Add a new lost or found item
app.post('/api/items', upload.single('image'), async (req, res) => {
  try {
    console.log('Add item request received:', req.body);
    console.log('File uploaded:', req.file);
    
    const { type, category, location, description, contact } = req.body;
    let image = '';
    if (req.file) {
      if (isVercel && req.file.buffer) {
        const mime = req.file.mimetype || 'image/jpeg';
        image = `data:${mime};base64,${req.file.buffer.toString('base64')}`;
      } else {
        image = req.file.filename;
      }
    }

    // Validate required fields
    if (!type || !category || !location) {
      console.log('Missing required fields');
      return res.status(400).json({
        success: false,
        message: 'Type, category, and location are required'
      });
    }

    const newItem = new Item({
      type,
      category,
      location,
      description: description || '',
      contact: contact || '',
      image
    });

    console.log('Saving item:', newItem);
    const savedItem = await newItem.save();
    console.log('Item saved successfully:', savedItem._id);
    
    res.status(201).json({
      success: true,
      message: 'Item added successfully',
      item: savedItem
    });
  } catch (error) {
    console.error('Error adding item:', error);
    res.status(400).json({
      success: false,
      message: 'Error adding item',
      error: error.message
    });
  }
});

// 2. Search items by location
app.get('/api/items/search', async (req, res) => {
  try {
    console.log('Search request received:', req.query);
    const { location } = req.query;
    
    if (!location) {
      console.log('No location provided');
      return res.status(400).json({
        success: false,
        message: 'Location parameter is required'
      });
    }

    console.log('Searching for location:', location);
    const items = await Item.find({
      location: { $regex: location, $options: 'i' }
    }).sort({ date: -1 });

    console.log('Found items:', items.length);
    res.json({
      success: true,
      items: items
    });
  } catch (error) {
    console.error('Search error:', error);
    res.status(500).json({
      success: false,
      message: 'Error searching items',
      error: error.message
    });
  }
});

// 3. List all items found in the last 7 days
app.get('/api/items/recent', async (req, res) => {
  try {
    const sevenDaysAgo = new Date();
    sevenDaysAgo.setDate(sevenDaysAgo.getDate() - 7);

    const recentItems = await Item.find({
      date: { $gte: sevenDaysAgo }
    }).sort({ date: -1 });

    res.json({
      success: true,
      items: recentItems
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Error fetching recent items',
      error: error.message
    });
  }
});

// 4. Match lost items with found items by category
app.get('/api/items/match', async (req, res) => {
  try {
    const { category } = req.query;
    
    if (!category) {
      return res.status(400).json({
        success: false,
        message: 'Category parameter is required'
      });
    }

    const lostItems = await Item.find({
      type: 'lost',
      category: { $regex: category, $options: 'i' }
    }).sort({ date: -1 });

    const foundItems = await Item.find({
      type: 'found',
      category: { $regex: category, $options: 'i' }
    }).sort({ date: -1 });

    res.json({
      success: true,
      matches: {
        lostItems,
        foundItems
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Error matching items',
      error: error.message
    });
  }
});

// Get all items
app.get('/api/items', async (req, res) => {
  try {
    const items = await Item.find().sort({ date: -1 });
    res.json({
      success: true,
      items: items
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Error fetching items',
      error: error.message
    });
  }
});

// Serve the main HTML page
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Handle 404 errors
app.use('*', (req, res) => {
  res.status(404).json({
    success: false,
    message: 'Route not found'
  });
});

// Local dev only; Vercel invokes this module as a serverless handler (no listen)
if (require.main === module) {
  app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
    console.log(`Visit http://localhost:${PORT} to view the application`);
  });
}

module.exports = app;
