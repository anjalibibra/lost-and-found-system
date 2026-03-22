# Code Explanation for Lost and Found Management System

This document explains what each file does and how the code works, so you can explain it to your teacher.

---

## 📁 **hash_table.c** - Hash Table Data Structure Implementation (C Language)

### **Purpose:**
This file implements a hash table data structure in C to demonstrate DSA (Data Structures and Algorithms) concepts. It shows how to efficiently store and retrieve lost/found items by category using hashing.

### **What the Code Does:**

#### **1. Data Structures Defined:**
- **`Item` struct** (lines 24-31): Represents a lost or found item with:
  - `type`: "lost" or "found"
  - `category`: Item category (e.g., "phone", "wallet")
  - `location`: Where item was lost/found
  - `description`: Item details
  - `contact`: Contact information
  - `timestamp`: When item was created

- **`HashNode` struct** (lines 34-38): A node in the hash table chain containing:
  - `key`: Category name (used for hashing)
  - `value`: Pointer to an Item
  - `next`: Pointer to next node (for collision handling)

- **`HashTable` struct** (lines 41-45): The main hash table structure with:
  - `buckets`: Array of pointers to HashNodes
  - `capacity`: Current size of the bucket array
  - `size`: Number of items currently stored

#### **2. Key Functions:**

**`create_hash_table()`** (lines 62-80):
- Allocates memory for a new hash table
- Initializes with 16 buckets (INITIAL_CAPACITY)
- Returns pointer to the created hash table

**`hash_function()`** (lines 102-111):
- Uses **djb2 algorithm** to convert category string into a hash value
- Returns an index within the bucket array range
- Formula: `hash = hash * 33 + character`

**`insert_item()`** (lines 114-150):
- Inserts a new item into the hash table
- Calculates hash index for the category
- Handles collisions using **chaining** (linked list at each bucket)
- Automatically resizes table if load factor exceeds 0.75
- Updates existing item if category already exists

**`find_item()`** (lines 153-167):
- Searches for an item by category
- Uses hash function to find the bucket
- Traverses the chain to find matching category
- Returns item pointer or NULL if not found
- **Time Complexity: O(1) average case**

**`delete_item()`** (lines 170-196):
- Removes an item from hash table
- Finds the item using hash function
- Removes node from chain and frees memory
- Updates table size

**`resize_table()`** (lines 199-226):
- Doubles the capacity when load factor > 0.75
- Rehashes all existing items into new buckets
- Prevents performance degradation as table grows

**`print_table()`** (lines 229-247):
- Displays the entire hash table structure
- Shows which items are in which buckets
- Useful for debugging and demonstration

**`demo_hash_table()`** (lines 311-358):
- Creates sample items (phone, wallet, keys, bag)
- Demonstrates insert, search, and delete operations
- Shows how hash table works in practice

### **Key Concepts Demonstrated:**
- ✅ Hash table data structure
- ✅ Collision resolution using chaining
- ✅ Dynamic resizing
- ✅ Memory management (malloc/free)
- ✅ String hashing algorithm
- ✅ O(1) average case lookup time

---

## 📁 **server.js** - Backend Server (Node.js + Express + MongoDB)

### **Purpose:**
This is the backend server that handles all API requests, connects to MongoDB database, and manages file uploads.

### **What the Code Does:**

#### **1. Setup and Configuration (lines 1-32):**
- Imports required modules: Express (web framework), Mongoose (MongoDB), CORS (cross-origin requests), Multer (file uploads)
- Creates Express app and sets port (3000)
- Configures middleware:
  - `cors()`: Allows frontend to make requests
  - `express.json()`: Parses JSON request bodies
  - `express.static('public')`: Serves static files (HTML, CSS, JS)

#### **2. File Upload Configuration (lines 16-25):**
- Configures Multer to save uploaded images
- Files saved to `public/uploads/` directory
- Filenames include timestamp to prevent conflicts

#### **3. Database Connection (lines 28-32):**
- Connects to MongoDB Atlas (cloud database)
- Uses connection string with credentials
- Logs success or error messages

#### **4. Database Schema (lines 35-67):**
- Defines `itemSchema` for MongoDB documents:
  - `type`: "lost" or "found" (required, enum)
  - `category`: Item category (required)
  - `location`: Where item was lost/found (required)
  - `date`: Timestamp (auto-generated)
  - `image`: Filename of uploaded image
  - `description`: Item description
  - `contact`: Contact information
- Creates `Item` model from schema

#### **5. API Routes:**

**POST `/api/items`** (lines 72-115):
- **Purpose**: Add a new lost or found item
- **Process**:
  1. Receives form data (including optional image file)
  2. Validates required fields (type, category, location)
  3. Saves image file if provided
  4. Creates new Item document in MongoDB
  5. Returns success message with saved item
- **Error Handling**: Returns 400 status if validation fails

**GET `/api/items/search`** (lines 118-149):
- **Purpose**: Search items by location
- **Process**:
  1. Receives location as query parameter
  2. Uses MongoDB regex search (case-insensitive)
  3. Sorts results by date (newest first)
  4. Returns matching items
- **Example**: `/api/items/search?location=Library`

**GET `/api/items/recent`** (lines 152-172):
- **Purpose**: Get items from last 7 days
- **Process**:
  1. Calculates date 7 days ago
  2. Queries MongoDB for items with date >= 7 days ago
  3. Returns sorted list (newest first)

**GET `/api/items/match`** (lines 175-210):
- **Purpose**: Match lost items with found items by category
- **Process**:
  1. Receives category as query parameter
  2. Finds all "lost" items with matching category
  3. Finds all "found" items with matching category
  4. Returns both lists together
- **Use Case**: Helps match lost items with found items

**GET `/api/items`** (lines 213-227):
- **Purpose**: Get all items in database
- **Process**: Fetches all items sorted by date

**GET `/`** (lines 230-232):
- **Purpose**: Serve the main HTML page
- Returns `index.html` file

#### **6. Error Handling:**
- 404 handler for unknown routes (lines 235-240)
- Try-catch blocks in all routes
- Proper HTTP status codes (200, 201, 400, 404, 500)

#### **7. Server Startup (lines 243-246):**
- Starts listening on specified port
- Logs server URL

### **Key Concepts Demonstrated:**
- ✅ RESTful API design
- ✅ MongoDB database operations (CRUD)
- ✅ File upload handling
- ✅ Middleware usage
- ✅ Error handling
- ✅ Async/await for database operations

---

## 📁 **public/index.html** - Frontend User Interface

### **Purpose:**
This is the main HTML page that users see and interact with. It provides the user interface for the Lost and Found system.

### **What the Code Does:**

#### **1. Document Structure (lines 1-8):**
- Standard HTML5 document
- Links to CSS file (`style.css`)
- Sets viewport for mobile responsiveness
- Sets page title

#### **2. Header Section (lines 11-14):**
- Displays application title
- Shows subtitle explaining the system

#### **3. Add New Item Form (lines 18-57):**
- **Form Fields**:
  - `type`: Dropdown to select "Lost Item" or "Found Item"
  - `category`: Text input for item category
  - `location`: Text input for location
  - `description`: Textarea for detailed description
  - `contact`: Text input for contact information
  - `image`: File input for uploading images
- **Form Attributes**:
  - `enctype="multipart/form-data"`: Required for file uploads
  - `id="itemForm"`: Used by JavaScript to handle submission
- **Submit Button**: Triggers form submission

#### **4. Search and Query Section (lines 60-92):**
Contains four query options:

**a) Search by Location (lines 64-70):**
- Text input for location
- Button to trigger search
- Results displayed in `locationResults` div

**b) Recent Items (lines 73-76):**
- Button to get items from last 7 days
- Results displayed in `recentResults` div

**c) Match Items by Category (lines 79-85):**
- Text input for category
- Button to find matches
- Results displayed in `matchResults` div

**d) All Items (lines 88-91):**
- Button to get all items
- Results displayed in `allResults` div

#### **5. Footer (lines 95-97):**
- Copyright information

#### **6. JavaScript Link (line 100):**
- Links to `script.js` for functionality

### **Key Concepts Demonstrated:**
- ✅ HTML5 semantic structure
- ✅ Form design and validation
- ✅ Responsive design considerations
- ✅ User interface layout

---

## 📁 **public/script.js** - Frontend JavaScript Logic

### **Purpose:**
This file contains all the JavaScript code that makes the frontend interactive. It handles form submissions, API calls, and displays results.

### **What the Code Does:**

#### **1. DOM Element References (lines 2-4):**
- Gets references to HTML elements:
  - `itemForm`: The add item form
  - `searchLocation`: Location search input
  - `matchCategory`: Category match input

#### **2. Event Listeners (lines 7-8):**
- Attaches submit handler to form
- Prevents default form submission behavior

#### **3. Form Submission Handler (lines 10-46):**
**`handleItemSubmit()` function:**
- Prevents default form submission
- Creates `FormData` object from form
- Sends POST request to `/api/items` with form data
- Handles response:
  - Shows success message if item added
  - Shows error message if failed
  - Resets form on success
- **Error Handling**: Checks for JSON response, handles network errors

#### **4. Search Functions:**

**`searchByLocation()`** (lines 49-84):
- Gets location from input field
- Validates input (checks if empty)
- Sends GET request to `/api/items/search?location=...`
- Displays results using `displayResults()` function
- Shows error message if search fails

**`getRecentItems()`** (lines 87-113):
- Sends GET request to `/api/items/recent`
- Displays items from last 7 days
- Handles errors gracefully

**`matchItems()`** (lines 116-149):
- Gets category from input
- Sends GET request to `/api/items/match?category=...`
- Displays both lost and found items using `displayMatchResults()`
- Shows separate sections for lost vs found items

**`getAllItems()`** (lines 152-178):
- Sends GET request to `/api/items`
- Displays all items in database
- Shows total count

#### **5. Display Functions:**

**`displayResults()`** (lines 181-196):
- Takes container ID, items array, and title
- Creates HTML for each item using `createItemCard()`
- Shows "No items found" if array is empty
- Updates the specified container with HTML

**`displayMatchResults()`** (lines 199-223):
- Similar to `displayResults()` but handles match structure
- Separates lost items and found items
- Shows counts for each category

**`createItemCard()`** (lines 226-246):
- Creates HTML card for a single item
- Includes:
  - Item type badge (lost/found with color coding)
  - Category name
  - Location, date, description, contact
  - Image if available
- Returns HTML string

#### **6. Utility Functions:**

**`showMessage()`** (lines 249-269):
- Displays temporary messages to user
- Supports "success" and "error" types
- Auto-removes after 5 seconds
- Inserts at top of page

#### **7. Initialization (lines 272-274):**
- Runs when page loads
- Logs message to console

### **Key Concepts Demonstrated:**
- ✅ DOM manipulation
- ✅ Event handling
- ✅ Fetch API for HTTP requests
- ✅ Async/await for asynchronous operations
- ✅ Error handling
- ✅ Dynamic HTML generation
- ✅ Form data handling

---

## 📁 **public/style.css** - Styling and Design

### **Purpose:**
This file contains all CSS styles to make the website look good and be user-friendly.

### **What the Code Does:**

#### **1. Reset and Base Styles (lines 1-13):**
- Resets default browser styles
- Sets base font and colors
- Sets background color

#### **2. Layout Styles:**
- **Container** (lines 15-19): Centers content, sets max width
- **Main** (lines 43-47): Two-column grid layout
- **Responsive** (lines 49-53): Single column on mobile devices

#### **3. Header Styling (lines 22-40):**
- Gradient background (purple to violet)
- White text
- Rounded corners
- Shadow effect
- Large, bold title

#### **4. Section Styling (lines 56-69):**
- White background cards
- Rounded corners
- Shadow for depth
- Colored headings with underline

#### **5. Form Styles (lines 72-104):**
- Spacing between form fields
- Input styling:
  - Padding and borders
  - Focus effects (border color changes)
  - Rounded corners
- Textarea with vertical resize

#### **6. Button Styles (lines 107-126):**
- Gradient background matching header
- Hover effect (lifts up with shadow)
- Active state (presses down)
- Smooth transitions

#### **7. Query Groups (lines 129-159):**
- Light gray background
- Colored left border
- Search form with flexbox layout
- Input and button side-by-side

#### **8. Results Display (lines 162-233):**
- Scrollable container (max height 400px)
- **Item Cards**:
  - White background
  - Border with color coding:
    - Red left border for "lost" items
    - Green left border for "found" items
  - Spacing and shadows
- **Item Header**: Flexbox layout for type badge and category
- **Type Badges**: Colored badges (red for lost, green for found)
- **Item Images**: Max size 100px, rounded corners

#### **9. Message Styles (lines 245-265):**
- **Success**: Green background, white text
- **Error**: Red background, white text
- Rounded corners, padding

#### **10. Responsive Design (lines 268-284):**
- Mobile-specific styles
- Smaller text on small screens
- Stacked form elements
- Full-width buttons

### **Key Concepts Demonstrated:**
- ✅ CSS Grid and Flexbox layouts
- ✅ Responsive design (mobile-first)
- ✅ Color coding for visual distinction
- ✅ Hover and focus effects
- ✅ Modern gradient designs
- ✅ Card-based UI design

---

## 📁 **package.json** - Project Dependencies

### **Purpose:**
This file defines the Node.js project configuration and lists all required packages.

### **What the Code Does:**

#### **1. Project Information (lines 2-4):**
- Project name and version
- Description of the project

#### **2. Scripts (lines 6-9):**
- **`start`**: Runs the server with `node server.js`
- **`dev`**: Runs server with nodemon (auto-restarts on changes)

#### **3. Dependencies (lines 10-17):**
- **express**: Web framework for Node.js
- **mongoose**: MongoDB object modeling
- **cors**: Enables cross-origin requests
- **multer**: Handles file uploads
- **mongodb**: MongoDB driver
- **node-fetch**: HTTP client (if needed)
- **path**: Path utilities (built-in, but listed)

#### **4. Dev Dependencies (lines 19-21):**
- **nodemon**: Development tool for auto-restart

#### **5. Metadata (lines 22-31):**
- Keywords for package search
- Author and license information

### **Key Concepts Demonstrated:**
- ✅ Node.js package management
- ✅ Dependency versioning
- ✅ Script definitions
- ✅ Project metadata

---

## 🔄 **How Everything Works Together:**

1. **User opens website** → `index.html` loads with `style.css` and `script.js`

2. **User fills form** → JavaScript captures form data

3. **Form submitted** → JavaScript sends POST request to `server.js`

4. **Server receives request** → Express middleware processes it

5. **File uploaded** → Multer saves image to `public/uploads/`

6. **Data saved** → Mongoose saves item to MongoDB Atlas

7. **Response sent** → Server sends success message back

8. **Frontend updates** → JavaScript displays success message

9. **User searches** → JavaScript sends GET request

10. **Server queries database** → MongoDB finds matching items

11. **Results displayed** → JavaScript creates HTML cards and shows them

---

## 📊 **Summary for Teacher:**

### **Technologies Used:**
- **Frontend**: HTML5, CSS3, JavaScript (Vanilla)
- **Backend**: Node.js, Express.js
- **Database**: MongoDB Atlas (NoSQL)
- **File Upload**: Multer
- **Data Structures**: Hash Table (C implementation)

### **Key Features:**
1. ✅ Add lost/found items with images
2. ✅ Search by location
3. ✅ View recent items (last 7 days)
4. ✅ Match lost items with found items
5. ✅ View all items
6. ✅ Hash table demonstration (C)

### **Learning Outcomes:**
- Web development (full-stack)
- RESTful API design
- Database operations (MongoDB)
- File handling
- Data structures (Hash Table)
- Memory management (C)
- Frontend-backend communication
- Error handling

---

This project demonstrates both **web development skills** (Node.js, MongoDB, HTML/CSS/JS) and **computer science fundamentals** (Data Structures and Algorithms with C implementation).

