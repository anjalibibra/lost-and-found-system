// DOM Elements
const itemForm = document.getElementById('itemForm');
const searchLocation = document.getElementById('searchLocation');
const matchCategory = document.getElementById('matchCategory');

// Event Listeners
itemForm.addEventListener('submit', handleItemSubmit);

// Handle form submission
async function handleItemSubmit(e) {
    e.preventDefault();
    
    const formData = new FormData(itemForm);
    
    try {
        const response = await fetch('/api/items', {
            method: 'POST',
            body: formData
        });
        
        // Check if response is ok
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        // Check if response is JSON
        const contentType = response.headers.get('content-type');
        if (!contentType || !contentType.includes('application/json')) {
            const text = await response.text();
            console.error('Non-JSON response:', text);
            throw new Error('Server returned non-JSON response');
        }
        
        const result = await response.json();
        
        if (result.success) {
            showMessage('Item added successfully!', 'success');
            itemForm.reset();
        } else {
            showMessage('Error adding item: ' + result.message, 'error');
        }
    } catch (error) {
        console.error('Add item error:', error);
        showMessage('Error adding item: ' + error.message, 'error');
    }
}

// Search items by location
async function searchByLocation() {
    const location = searchLocation.value.trim();
    
    if (!location) {
        showMessage('Please enter a location to search', 'error');
        return;
    }
    
    try {
        const response = await fetch(`/api/items/search?location=${encodeURIComponent(location)}`);
        
        // Check if response is ok
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        // Check if response is JSON
        const contentType = response.headers.get('content-type');
        if (!contentType || !contentType.includes('application/json')) {
            const text = await response.text();
            console.error('Non-JSON response:', text);
            throw new Error('Server returned non-JSON response');
        }
        
        const result = await response.json();
        
        if (result.success) {
            displayResults('locationResults', result.items, 'Items found in ' + location);
        } else {
            showMessage('Error searching items: ' + result.message, 'error');
        }
    } catch (error) {
        console.error('Search error:', error);
        showMessage('Error searching items: ' + error.message, 'error');
    }
}

// Get recent items (last 7 days)
async function getRecentItems() {
    try {
        const response = await fetch('/api/items/recent');
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const contentType = response.headers.get('content-type');
        if (!contentType || !contentType.includes('application/json')) {
            const text = await response.text();
            console.error('Non-JSON response:', text);
            throw new Error('Server returned non-JSON response');
        }
        
        const result = await response.json();
        
        if (result.success) {
            displayResults('recentResults', result.items, 'Recent Items (Last 7 Days)');
        } else {
            showMessage('Error fetching recent items: ' + result.message, 'error');
        }
    } catch (error) {
        console.error('Recent items error:', error);
        showMessage('Error fetching recent items: ' + error.message, 'error');
    }
}

// Match items by category
async function matchItems() {
    const category = matchCategory.value.trim();
    
    if (!category) {
        showMessage('Please enter a category to match', 'error');
        return;
    }
    
    try {
        const response = await fetch(`/api/items/match?category=${encodeURIComponent(category)}`);
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const contentType = response.headers.get('content-type');
        if (!contentType || !contentType.includes('application/json')) {
            const text = await response.text();
            console.error('Non-JSON response:', text);
            throw new Error('Server returned non-JSON response');
        }
        
        const result = await response.json();
        
        if (result.success) {
            displayMatchResults('matchResults', result.matches, 'Matches for ' + category);
        } else {
            showMessage('Error matching items: ' + result.message, 'error');
        }
    } catch (error) {
        console.error('Match items error:', error);
        showMessage('Error matching items: ' + error.message, 'error');
    }
}

// Get all items
async function getAllItems() {
    try {
        const response = await fetch('/api/items');
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const contentType = response.headers.get('content-type');
        if (!contentType || !contentType.includes('application/json')) {
            const text = await response.text();
            console.error('Non-JSON response:', text);
            throw new Error('Server returned non-JSON response');
        }
        
        const result = await response.json();
        
        if (result.success) {
            displayResults('allResults', result.items, 'All Items');
        } else {
            showMessage('Error fetching items: ' + result.message, 'error');
        }
    } catch (error) {
        console.error('Get all items error:', error);
        showMessage('Error fetching items: ' + error.message, 'error');
    }
}

// Display search results
function displayResults(containerId, items, title) {
    const container = document.getElementById(containerId);
    
    if (items.length === 0) {
        container.innerHTML = `<div class="no-results">No items found.</div>`;
        return;
    }
    
    let html = `<h4>${title} (${items.length} items)</h4>`;
    
    items.forEach(item => {
        html += createItemCard(item);
    });
    
    container.innerHTML = html;
}

// Display match results
function displayMatchResults(containerId, matches, title) {
    const container = document.getElementById(containerId);
    
    let html = `<h4>${title}</h4>`;
    
    if (matches.lostItems.length > 0) {
        html += `<h5>Lost Items (${matches.lostItems.length})</h5>`;
        matches.lostItems.forEach(item => {
            html += createItemCard(item);
        });
    }
    
    if (matches.foundItems.length > 0) {
        html += `<h5>Found Items (${matches.foundItems.length})</h5>`;
        matches.foundItems.forEach(item => {
            html += createItemCard(item);
        });
    }
    
    if (matches.lostItems.length === 0 && matches.foundItems.length === 0) {
        html += `<div class="no-results">No matching items found.</div>`;
    }
    
    container.innerHTML = html;
}

// Create item card HTML
function createItemCard(item) {
    const date = new Date(item.date).toLocaleDateString();
    const time = new Date(item.date).toLocaleTimeString();
    const imageSrc = item.image
      ? (item.image.startsWith('data:') ? item.image : `/uploads/${item.image}`)
      : '';
    const imageHtml = imageSrc ? `<img src="${imageSrc}" alt="Item image" class="item-image">` : '';
    
    return `
        <div class="item-card ${item.type}">
            <div class="item-header">
                <span class="item-type ${item.type}">${item.type}</span>
                <span class="item-category">${item.category}</span>
            </div>
            <div class="item-details">
                <p><strong>Location:</strong> ${item.location}</p>
                <p><strong>Date:</strong> ${date} at ${time}</p>
                ${item.description ? `<p><strong>Description:</strong> ${item.description}</p>` : ''}
                ${item.contact ? `<p><strong>Contact:</strong> ${item.contact}</p>` : ''}
            </div>
            ${imageHtml}
        </div>
    `;
}

// Show message to user
function showMessage(message, type) {
    // Remove existing messages
    const existingMessages = document.querySelectorAll('.message');
    existingMessages.forEach(msg => msg.remove());
    
    // Create new message
    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${type}`;
    messageDiv.textContent = message;
    
    // Insert at the top of the container
    const container = document.querySelector('.container');
    container.insertBefore(messageDiv, container.firstChild);
    
    // Auto remove after 5 seconds
    setTimeout(() => {
        if (messageDiv.parentNode) {
            messageDiv.remove();
        }
    }, 5000);
}

// Initialize the page
document.addEventListener('DOMContentLoaded', function() {
    console.log('Lost and Found Management System loaded');
});
