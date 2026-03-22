# Lost and Found Management System (Node + Express + MongoDB Atlas)

Simple lost-and-found app:
- Backend: Node.js + Express + Mongoose
- Database: MongoDB Atlas
- Frontend: static HTML/CSS/JS served from `public/`

## Requirements

- Node.js 18+ (recommended)
- A MongoDB Atlas cluster (free tier works)

## Project structure

- `server.js`: Express server + API + MongoDB connection
- `public/`: frontend assets (served by Express)
- `public/uploads/`: uploaded images (created locally at runtime)

## Setup (local)

### 1) Install dependencies

```bash
npm install
```

### 2) Create uploads folder (important for image upload)

```bash
mkdir -p public/uploads
```

> On Windows PowerShell you can run: `mkdir public\uploads`

### 3) Configure environment variables

This project reads:
- `MONGODB_URI` (MongoDB Atlas connection string)
- `PORT` (optional, defaults to `3000`)

Create a `.env` file (recommended) or set env vars in your shell.

See `.env.example`.

### 4) Run the server

Development (auto-restart):

```bash
npm run dev
```

Production-like:

```bash
npm start
```

Open:
- `http://localhost:3000`

## MongoDB Atlas setup

### 1) Create cluster

In MongoDB Atlas:
- Create a Project
- Build a database (Shared/Free tier is OK)

### 2) Create database user

Atlas → **Database Access** → **Add New Database User**
- Username + password

### 3) Allow network access

Atlas → **Network Access** → **Add IP Address**
- For quick testing: allow `0.0.0.0/0` (not ideal for real production)
- Or add your current IP

### 4) Get your connection string

Atlas → **Database** → **Connect** → **Drivers (Node.js)**

You’ll get a URI like:

`mongodb+srv://<user>:<password>@<cluster>.mongodb.net/<dbName>?retryWrites=true&w=majority`

Set:
- `<dbName>` to `lostandfound` (or any DB name you want)

## Deploy (GitHub + Render recommended)

You said you’ll deploy after pushing to GitHub. These steps assume **Render** (easy for Node apps).

### 1) Initialize git and push to GitHub

```bash
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin <YOUR_GITHUB_REPO_URL>
git push -u origin main
```

### 2) Create Render Web Service

On Render:
- New → **Web Service**
- Connect your GitHub repo
- Environment: **Node**
- Build Command: `npm install`
- Start Command: `npm start`

### 3) Add environment variables on Render

Render → Service → **Environment**
- `MONGODB_URI` = your Atlas connection string
- (Optional) `PORT` is set automatically by Render; you don’t need to add it

### 4) Configure Atlas Network Access for Render

If your app can’t connect from Render:
- In Atlas Network Access, temporarily allow `0.0.0.0/0`
- Or (better) add the static outbound IPs if your plan/platform provides them (Render usually doesn’t for free tiers)

### 5) Verify

Open your Render URL. The home page is served from `public/index.html`.

## API endpoints

- `POST /api/items` (multipart/form-data; field `image` optional)
- `GET /api/items`
- `GET /api/items/search?location=<text>`
- `GET /api/items/recent`
- `GET /api/items/match?category=<text>`

## Notes / gotchas

- Don’t commit secrets: keep Atlas URI in env vars, not in code.
- Ensure `public/uploads/` exists and is writable in the environment.
  - On many platforms, local disk is **ephemeral**; uploaded images may be wiped on redeploy.
  - For a real deployment, store images in object storage (S3, Cloudinary, etc.).

# Lost and Found Management System

A comprehensive web application for managing lost and found items with a Node.js backend, MongoDB Atlas database, and a simple HTML frontend. This project also includes a C implementation of a hash table for efficient item lookup by category.

## Features

### Backend (Node.js + Express + MongoDB)
- **Add Items**: Submit lost or found items with details like category, location, description, and contact info
- **Search by Location**: Find items based on where they were lost or found
- **Recent Items**: View all items added in the last 7 days
- **Item Matching**: Match lost items with found items by category
- **Image Upload**: Support for uploading images of items
- **RESTful API**: Clean API endpoints for all operations

### Frontend (HTML + CSS + JavaScript)
- **Simple Interface**: Clean, responsive design with forms and buttons
- **Real-time Updates**: Dynamic content loading without page refresh
- **Image Display**: Show uploaded images for items
- **Search Functionality**: Easy search and filtering options

### Data Structures (C Implementation)
- **Hash Table**: Efficient O(1) average case lookup for items by category
- **Collision Resolution**: Chaining method for handling hash collisions
- **Dynamic Resizing**: Automatic table expansion when load factor exceeds threshold
- **Memory Management**: Proper allocation and deallocation of memory

## Project Structure

```
dsaDbmsProject/
├── server.js              # Main Express server
├── package.json           # Node.js dependencies
├── hash_table.c           # C implementation of hash table
├── README.md              # This file
└── public/                # Frontend files
    ├── index.html         # Main HTML page
    ├── style.css          # CSS styling
    ├── script.js          # JavaScript functionality
    └── uploads/           # Directory for uploaded images
```

## Prerequisites

- Node.js (v14 or higher)
- MongoDB Atlas account
- C compiler (gcc, clang, or Visual Studio)
- Git

## Installation & Setup

### 1. Clone the Repository
```bash
git clone <your-repository-url>
cd dsaDbmsProject
```

### 2. Install Node.js Dependencies
```bash
npm install
```

### 3. Set up MongoDB Atlas

1. Go to [MongoDB Atlas](https://www.mongodb.com/atlas)
2. Create a free account and cluster
3. Get your connection string
4. Update the `MONGODB_URI` in `server.js` with your connection string:

```javascript
const MONGODB_URI = 'mongodb+srv://username:password@cluster.mongodb.net/lostandfound?retryWrites=true&w=majority';
```

### 4. Start the Application

```bash
npm start
```

The application will be available at `http://localhost:3000`

For development with auto-restart:
```bash
npm run dev
```

### 5. Compile and Run the C Hash Table

```bash
# Compile the C program
gcc -o hash_table hash_table.c

# Run the demo
./hash_table
```

On Windows with Visual Studio:
```cmd
cl hash_table.c
hash_table.exe
```

## API Endpoints

### Add New Item
- **POST** `/api/items`
- **Body**: Form data with type, category, location, description, contact, and optional image
- **Response**: Success message with created item

### Search by Location
- **GET** `/api/items/search?location=<location>`
- **Response**: Array of items matching the location

### Get Recent Items
- **GET** `/api/items/recent`
- **Response**: Array of items from the last 7 days

### Match Items by Category
- **GET** `/api/items/match?category=<category>`
- **Response**: Object with lostItems and foundItems arrays

### Get All Items
- **GET** `/api/items`
- **Response**: Array of all items

## Database Schema

### Items Collection
```javascript
{
  type: String,        // "lost" or "found"
  category: String,    // Item category (e.g., "phone", "wallet")
  location: String,    // Where item was lost/found
  date: Date,          // Timestamp (auto-generated)
  image: String,       // Filename of uploaded image
  description: String, // Item description
  contact: String      // Contact information
}
```

## Hash Table Implementation (C)

The C implementation demonstrates efficient data structure usage for quick item lookup:

### Features
- **Dynamic Sizing**: Automatically resizes when load factor exceeds 0.75
- **Collision Resolution**: Uses chaining to handle hash collisions
- **Memory Management**: Proper allocation and cleanup
- **String Hashing**: Uses djb2 algorithm for string keys

### Key Functions
- `create_hash_table()`: Initialize new hash table
- `insert_item()`: Add item to hash table
- `find_item()`: Search for item by category
- `delete_item()`: Remove item from hash table
- `resize_table()`: Expand table when needed

### Time Complexity
- **Insert**: O(1) average, O(n) worst case
- **Search**: O(1) average, O(n) worst case
- **Delete**: O(1) average, O(n) worst case

## Usage Examples

### Adding a Lost Item
1. Open the web application
2. Select "Lost Item" from the type dropdown
3. Enter category (e.g., "phone")
4. Enter location (e.g., "Library")
5. Add description and contact info
6. Optionally upload an image
7. Click "Add Item"

### Searching for Items
1. Use the "Search by Location" section
2. Enter a location (e.g., "Library")
3. Click "Search" to see all items from that location

### Finding Matches
1. Use the "Match Items by Category" section
2. Enter a category (e.g., "phone")
3. Click "Find Matches" to see both lost and found items of that category

## Development

### Adding New Features
1. Update the MongoDB schema in `server.js` if needed
2. Add new API routes in `server.js`
3. Update the frontend in `public/script.js` and `public/index.html`
4. Test thoroughly

### Database Modifications
- The MongoDB schema is defined in the `itemSchema` in `server.js`
- Add new fields by updating the schema
- Update the frontend forms to include new fields

## Troubleshooting

### Common Issues

1. **MongoDB Connection Error**
   - Verify your connection string
   - Check if your IP is whitelisted in MongoDB Atlas
   - Ensure your cluster is running

2. **Image Upload Issues**
   - Check if the `public/uploads` directory exists
   - Verify file permissions
   - Ensure multer is properly configured

3. **C Compilation Errors**
   - Make sure you have a C compiler installed
   - Check for syntax errors in `hash_table.c`
   - Ensure all required headers are included

### Debug Mode
Run the server with debug logging:
```bash
DEBUG=* npm start
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Future Enhancements

- User authentication and authorization
- Email notifications for matches
- Advanced search filters
- Mobile app development
- Real-time notifications
- Item status tracking
- Admin dashboard
- Analytics and reporting

## Contact

For questions or support, please contact [your-email@example.com]

---

**Note**: This project demonstrates both web development skills (Node.js, MongoDB, HTML/CSS/JS) and computer science fundamentals (Data Structures and Algorithms with C implementation).
