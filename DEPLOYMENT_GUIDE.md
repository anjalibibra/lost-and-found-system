# Deployment Guide

This guide will help you upload your code to GitHub and deploy your website.

## ⚠️ Important: This is NOT a Static Website

Your project uses Node.js + Express + MongoDB, so it **cannot** be hosted on GitHub Pages (which only supports static HTML/CSS/JS).

You need to:
1. **Upload code to GitHub** (code repository)
2. **Deploy to a hosting service** (live website)

---

## Step 1: Upload Code to GitHub

### Option A: Using GitHub Desktop (Easiest)
1. Download and install [GitHub Desktop](https://desktop.github.com/)
2. Open GitHub Desktop
3. Go to File → Add Local Repository
4. Choose your `dsaDbmsProject` folder
5. Click "Publish repository"
6. Choose a name and make it public or private
7. Click "Publish"

### Option B: Using Git Command Line
If you have Git installed, run these commands in your terminal:

```bash
# 1. Initialize git repository
git init

# 2. Add all files
git add .

# 3. Commit files
git commit -m "Initial commit"

# 4. Create repository on GitHub.com first, then connect
git remote add origin https://github.com/YOUR-USERNAME/YOUR-REPO-NAME.git

# 5. Push to GitHub
git branch -M main
git push -u origin main
```

**Note:** You'll need to create the repository on GitHub.com first, then copy the repository URL.

---

## Step 2: Deploy Your Website (Live Hosting)

Since you can't use GitHub Pages, here are free hosting options:

### Option 1: Render (Recommended - Easiest)
1. Go to [render.com](https://render.com) and sign up with GitHub
2. Click "New Web Service"
3. Connect your GitHub repository
4. Render will auto-detect it's a Node.js app
5. Click "Create Web Service"
6. Your site will be live at: `your-app.onrender.com`

**Environment Variables to set in Render:**
- `MONGODB_URI`: Your MongoDB connection string (already in code)
- `PORT`: Leave empty (Render sets this automatically)

### Option 2: Railway
1. Go to [railway.app](https://railway.app)
2. Sign up with GitHub
3. Click "New Project" → "Deploy from GitHub"
4. Select your repository
5. Railway will automatically deploy

### Option 3: Heroku
1. Go to [heroku.com](https://www.heroku.com)
2. Create account and install Heroku CLI
3. Run:
```bash
heroku login
heroku create your-app-name
git push heroku main
```

---

## Step 3: Configure MongoDB

Your code already has MongoDB connection configured:
```javascript
const MONGODB_URI = process.env.MONGODB_URI || 'mongodb+srv://admin:admin@cluster0.rmemgma.mongodb.net/lostandfound?retryWrites=true&w=majority&appName=Cluster0';
```

Make sure your MongoDB Atlas:
- ✅ Has IP whitelist set to `0.0.0.0/0` (allow all IPs for hosting)
- ✅ Has a database user with proper permissions

---

## Step 4: Set Environment Variables

When deploying to hosting services, set these in the dashboard:
- `MONGODB_URI`: Your MongoDB connection string
- `PORT`: Leave empty (hosting service sets this)

---

## Quick Start Checklist

- [ ] Create GitHub account
- [ ] Create a new repository on GitHub
- [ ] Upload your code (using GitHub Desktop or Git commands)
- [ ] Sign up for a hosting service (Render/Railway/Heroku)
- [ ] Connect GitHub repository
- [ ] Deploy!
- [ ] Update MongoDB Atlas IP whitelist to allow hosting service

---

## Your Repository Structure

Once uploaded to GitHub, your repository will contain:
```
your-repo/
├── .gitignore          # Files to ignore
├── server.js            # Main server file
├── package.json         # Dependencies
├── hash_table.c         # C implementation
├── README.md            # Project documentation
└── public/              # Frontend files
    ├── index.html
    ├── script.js
    ├── style.css
    └── uploads/         # Upload folder (empty in repo)
```

---

## Troubleshooting

### MongoDB Connection Issues
- Check if your IP is whitelisted in MongoDB Atlas
- Add `0.0.0.0/0` to allow all IPs (for hosting services)
- Verify your connection string

### Upload Directory Issues
- Some hosting services don't allow file uploads on free tier
- Consider using cloud storage (AWS S3, Cloudinary) for images

### Port Issues
- Don't hardcode port, use `process.env.PORT || 3000`
- Already configured in your `server.js`

---

## Need Help?

- GitHub Docs: https://docs.github.com
- Render Docs: https://render.com/docs
- Railway Docs: https://docs.railway.app
- Heroku Docs: https://devcenter.heroku.com

