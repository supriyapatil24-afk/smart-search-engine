// API Configuration
const API_BASE_URL = "http://localhost:8080/api";

// DOM Elements
const pages = {
  search: document.getElementById("search-page"),
  upload: document.getElementById("upload-page"),
  learning: document.getElementById("learning-page"),
  mindmap: document.getElementById("mindmap-page"),
};

// Global state
let uploadedFiles = [];

// ========== INITIALIZATION ==========
document.addEventListener("DOMContentLoaded", async () => {
  // Set up navigation
  setupNavigation();

  // Load initial stats
  await loadStats();

  // Set up search
  setupSearch();

  // Set up upload
  setupUpload();

  // Set up learning path
  setupLearningPath();

  // Set up mind map
  setupMindMap();
});

// ========== NAVIGATION ==========
function setupNavigation() {
  document.querySelectorAll(".nav-link").forEach((link) => {
    link.addEventListener("click", (e) => {
      e.preventDefault();
      const page = e.target.closest(".nav-link").dataset.page;

      // Update active navigation
      document
        .querySelectorAll(".nav-link")
        .forEach((nav) => nav.classList.remove("active"));
      e.target.closest(".nav-link").classList.add("active");

      // Show selected page
      Object.values(pages).forEach((p) => p.classList.remove("active"));
      pages[page].classList.add("active");

      // Load data for the page if needed
      if (page === "search") {
        loadStats();
      }
    });
  });
}

// ========== STATS ==========
async function loadStats() {
  try {
    const response = await fetch(`${API_BASE_URL}/stats`);
    if (!response.ok) throw new Error("Failed to load stats");

    const data = await response.json();

    // Update UI
    document.getElementById("total-files").textContent = data.totalFiles || 0;
    document.getElementById("total-keywords").textContent = data.totalFiles
      ? data.totalFiles * 50
      : 0; // Estimated

    // Store uploaded files
    if (data.uploadedFiles) {
      uploadedFiles = data.uploadedFiles;
      updateFilesList();
    }
  } catch (error) {
    console.error("Error loading stats:", error);
    document.getElementById("total-files").textContent = "0";
  }
}

function updateFilesList() {
  const filesList = document.getElementById("files-list");
  filesList.innerHTML = "";

  if (uploadedFiles.length === 0) {
    filesList.innerHTML = `
            <div class="empty-state">
                <i class="fas fa-file"></i>
                <p>No files uploaded yet</p>
            </div>
        `;
    return;
  }

  uploadedFiles.forEach((filename) => {
    const fileItem = document.createElement("div");
    fileItem.className = "file-item";
    fileItem.innerHTML = `
            <div class="file-info">
                <h4>${filename}</h4>
                <p><i class="fas fa-clock"></i> Uploaded recently</p>
            </div>
            <div class="file-status">
                <span class="result-badge"><i class="fas fa-check"></i> Processed</span>
            </div>
        `;
    filesList.appendChild(fileItem);
  });
}

// ========== SEARCH ==========
function setupSearch() {
  const searchBtn = document.getElementById("search-btn");
  const searchInput = document.getElementById("search-input");

  searchBtn.addEventListener("click", performSearch);
  searchInput.addEventListener("keypress", (e) => {
    if (e.key === "Enter") performSearch();
  });
}

async function performSearch() {
  const query = document.getElementById("search-input").value.trim();
  if (!query) {
    alert("Please enter a search term");
    return;
  }

  const searchBtn = document.getElementById("search-btn");
  const originalText = searchBtn.innerHTML;
  searchBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Searching...';
  searchBtn.disabled = true;

  try {
    const response = await fetch(
      `${API_BASE_URL}/search?q=${encodeURIComponent(query)}`
    );
    if (!response.ok) throw new Error("Search failed");

    const data = await response.json();
    displaySearchResults(data);
  } catch (error) {
    console.error("Search error:", error);
    showError("Search failed. Make sure the server is running.");
  } finally {
    searchBtn.innerHTML = originalText;
    searchBtn.disabled = false;
  }
}

function displaySearchResults(data) {
  const resultsContainer = document.getElementById("search-results");
  const title = document.getElementById("results-title");
  const relatedContainer = document.getElementById("related-topics");

  title.classList.remove("hidden");
  title.innerHTML = `Search Results for "${data.query}" (${data.total} found)`;

  // Display search results
  resultsContainer.innerHTML = "";
  if (data.results && data.results.length > 0) {
    data.results.forEach((result, index) => {
      const resultCard = document.createElement("div");
      resultCard.className = "result-card";
      resultCard.innerHTML = `
                <div class="result-header">
                    <h3 class="result-title">#${index + 1} ${
        result.filename
      }</h3>
                    <span class="result-badge">${
                      result.frequency
                    } mentions</span>
                </div>
                <p class="result-snippet">${
                  result.snippet || "No snippet available"
                }</p>
            `;
      resultsContainer.appendChild(resultCard);
    });
  } else {
    resultsContainer.innerHTML = `
            <div class="empty-state">
                <i class="fas fa-search"></i>
                <h3>No Results Found</h3>
                <p>Try a different search term or upload more notes</p>
            </div>
        `;
  }

  // Display related topics
  relatedContainer.innerHTML = "";
  if (data.related && data.related.length > 0) {
    data.related.forEach((topic) => {
      const tag = document.createElement("span");
      tag.className = "topic-tag";
      tag.textContent = `${topic.topic} (${topic.weight})`;
      relatedContainer.appendChild(tag);
    });
  } else {
    relatedContainer.innerHTML =
      '<p class="empty-text">No related topics found</p>';
  }
}

// ========== UPLOAD ==========
function setupUpload() {
  const dropArea = document.getElementById("drop-area");
  const fileInput = document.getElementById("file-input");

  // Drag and drop
  ["dragenter", "dragover", "dragleave", "drop"].forEach((eventName) => {
    dropArea.addEventListener(eventName, preventDefaults, false);
  });

  function preventDefaults(e) {
    e.preventDefault();
    e.stopPropagation();
  }

  ["dragenter", "dragover"].forEach((eventName) => {
    dropArea.addEventListener(
      eventName,
      () => {
        dropArea.style.backgroundColor = "rgba(102, 126, 234, 0.1)";
      },
      false
    );
  });

  ["dragleave", "drop"].forEach((eventName) => {
    dropArea.addEventListener(
      eventName,
      () => {
        dropArea.style.backgroundColor = "";
      },
      false
    );
  });

  dropArea.addEventListener("drop", handleDrop, false);
  dropArea.addEventListener("click", () => fileInput.click());
  fileInput.addEventListener("change", handleFileSelect);
}

function handleDrop(e) {
  const dt = e.dataTransfer;
  const files = dt.files;
  uploadFile(files[0]);
}

function handleFileSelect(e) {
  const files = e.target.files;
  if (files.length > 0) {
    uploadFile(files[0]);
  }
}

async function uploadFile(file) {
  if (!file) return;

  const progressContainer = document.getElementById("progress-container");
  const progressFill = document.getElementById("progress-fill");
  const progressText = document.getElementById("progress-text");

  // Show progress
  progressContainer.classList.remove("hidden");
  progressFill.style.width = "0%";
  progressText.textContent = "0%";

  // Create form data
  const formData = new FormData();
  formData.append("file", file);

  try {
    // Upload to server
    const response = await fetch(`${API_BASE_URL}/upload`, {
      method: "POST",
      body: formData,
    });

    if (!response.ok) {
      throw new Error("Upload failed");
    }

    const data = await response.json();

    // Update progress
    progressFill.style.width = "100%";
    progressText.textContent = "100%";

    // Show success message
    setTimeout(() => {
      progressContainer.classList.add("hidden");
      showSuccess(`File "${file.name}" uploaded successfully!`);

      // Reset file input
      document.getElementById("file-input").value = "";

      // Reload stats and files list
      loadStats();
    }, 500);
  } catch (error) {
    console.error("Upload error:", error);
    progressContainer.classList.add("hidden");
    showError("Upload failed. Make sure the server is running.");
  }
}

// ========== LEARNING PATH ==========
function setupLearningPath() {
  document
    .getElementById("generate-path-btn")
    .addEventListener("click", generateLearningPath);
}

async function generateLearningPath() {
  const topic = document.getElementById("topic-input").value.trim();
  if (!topic) {
    showError("Please enter a topic");
    return;
  }

  const btn = document.getElementById("generate-path-btn");
  const originalText = btn.innerHTML;
  btn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Generating...';
  btn.disabled = true;

  try {
    const response = await fetch(
      `${API_BASE_URL}/learning-path?topic=${encodeURIComponent(topic)}`
    );
    if (!response.ok) throw new Error("Failed to generate learning path");

    const data = await response.json();
    displayLearningPath(data);
  } catch (error) {
    console.error("Learning path error:", error);
    showError(
      "Failed to generate learning path. Topic may not exist in database."
    );

    document.getElementById("no-path").classList.remove("hidden");
    document.getElementById("path-container").classList.add("hidden");
  } finally {
    btn.innerHTML = originalText;
    btn.disabled = false;
  }
}

function displayLearningPath(data) {
  const pathContainer = document.getElementById("path-container");
  const stepsContainer = document.querySelector(".path-steps");
  const title = document.querySelector(".path-title");

  document.getElementById("no-path").classList.add("hidden");
  pathContainer.classList.remove("hidden");

  title.innerHTML = `Learning Path: <span style="color: #667eea">${data.topic}</span>`;

  stepsContainer.innerHTML = "";
  if (data.path && data.path.length > 0) {
    data.path.forEach((step, index) => {
      const stepDiv = document.createElement("div");
      stepDiv.className = "path-step";

      const nextTopic =
        index < data.path.length - 1 ? data.path[index + 1].topic : null;

      stepDiv.innerHTML = `
                <div class="step-number">${step.order}</div>
                <div class="step-content">
                    <h3>${step.topic}</h3>
                    <p>Focus on understanding <strong>${step.topic}</strong> ${
        nextTopic
          ? `before moving to <strong>${nextTopic}</strong>`
          : "(final topic in sequence)"
      }</p>
                </div>
            `;
      stepsContainer.appendChild(stepDiv);
    });
  } else {
    stepsContainer.innerHTML = `
            <div class="empty-state">
                <i class="fas fa-exclamation-triangle"></i>
                <h3>No Learning Path Found</h3>
                <p>Try uploading notes containing this topic first</p>
            </div>
        `;
  }
}

// ========== MIND MAP ==========
function setupMindMap() {
  const depthSlider = document.getElementById("depth-slider");
  const depthValue = document.getElementById("depth-value");
  const generateBtn = document.getElementById("generate-mindmap-btn");

  depthSlider.addEventListener("input", (e) => {
    depthValue.textContent = e.target.value;
  });

  generateBtn.addEventListener("click", generateMindMap);
}

async function generateMindMap() {
  const topic = document.getElementById("mindmap-topic").value.trim();
  const depth = document.getElementById("depth-slider").value;

  if (!topic) {
    showError("Please enter a topic");
    return;
  }

  const btn = document.getElementById("generate-mindmap-btn");
  const originalText = btn.innerHTML;
  btn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Generating...';
  btn.disabled = true;

  try {
    const response = await fetch(
      `${API_BASE_URL}/mindmap?topic=${encodeURIComponent(
        topic
      )}&depth=${depth}`
    );
    if (!response.ok) throw new Error("Failed to generate mind map");

    const data = await response.json();
    drawMindMap(data);
    displayConnections(data);

    document.getElementById("no-mindmap").classList.add("hidden");
  } catch (error) {
    console.error("Mind map error:", error);
    showError("Failed to generate mind map. Topic may not exist in database.");
    document.getElementById("no-mindmap").classList.remove("hidden");
  } finally {
    btn.innerHTML = originalText;
    btn.disabled = false;
  }
}

function drawMindMap(data) {
  const canvas = document.getElementById("mindmap-canvas");
  const ctx = canvas.getContext("2d");

  // Clear canvas
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const centerX = canvas.width / 2;
  const centerY = canvas.height / 2;

  // Draw center node
  drawNode(ctx, centerX, centerY, data.center, true);

  // Draw connections
  if (data.connections && data.connections.length > 0) {
    const radius = Math.min(200, data.connections.length * 30);
    const angleStep = (2 * Math.PI) / data.connections.length;

    data.connections.forEach((conn, index) => {
      const angle = index * angleStep;
      const x = centerX + radius * Math.cos(angle);
      const y = centerY + radius * Math.sin(angle);

      // Draw connection line with weight-based thickness
      ctx.beginPath();
      ctx.moveTo(centerX, centerY);
      ctx.lineTo(x, y);
      ctx.strokeStyle = `rgba(102, 126, 234, ${
        0.3 + Math.min(conn.weight, 10) * 0.05
      })`;
      ctx.lineWidth = Math.max(1, Math.min(conn.weight, 5));
      ctx.stroke();

      // Draw connection node
      drawNode(ctx, x, y, `${conn.topic}\n(${conn.weight})`, false);
    });
  }
}

function drawNode(ctx, x, y, text, isCenter) {
  const radius = isCenter ? 60 : 40;

  // Draw circle
  ctx.beginPath();
  ctx.arc(x, y, radius, 0, Math.PI * 2);
  ctx.fillStyle = isCenter
    ? "rgba(102, 126, 234, 0.9)"
    : "rgba(255, 255, 255, 0.9)";
  ctx.fill();
  ctx.strokeStyle = isCenter ? "#fff" : "#667eea";
  ctx.lineWidth = 2;
  ctx.stroke();

  // Draw text
  ctx.fillStyle = isCenter ? "#fff" : "#333";
  ctx.font = isCenter ? "bold 16px Arial" : "14px Arial";
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";

  const lines = text.split("\n");
  lines.forEach((line, i) => {
    ctx.fillText(line, x, y + (i - (lines.length - 1) / 2) * 18);
  });
}

function displayConnections(data) {
  const connectionsInfo = document.getElementById("connections-info");
  const connectionsList = document.getElementById("connections-list");

  connectionsInfo.innerHTML = `
        <p><strong>${data.center}</strong> connects to ${data.connections.length} topics.</p>
        <p>Connection strength (weight) shows how often topics appear together.</p>
    `;

  connectionsList.innerHTML = "";
  if (data.connections && data.connections.length > 0) {
    // Sort by weight
    const sortedConnections = [...data.connections].sort(
      (a, b) => b.weight - a.weight
    );

    sortedConnections.forEach((conn) => {
      const tag = document.createElement("span");
      tag.className = "connection-tag";

      // Color code by weight
      if (conn.weight >= 3) {
        tag.style.background =
          "linear-gradient(135deg, #667eea 0%, #764ba2 100%)";
      } else if (conn.weight >= 2) {
        tag.style.background =
          "linear-gradient(135deg, #5d9cec 0%, #6a7be4 100%)";
      } else {
        tag.style.background = "linear-gradient(135deg, #aaa 0%, #888 100%)";
      }

      tag.textContent = `${conn.topic} (${conn.weight})`;
      connectionsList.appendChild(tag);
    });
  } else {
    connectionsList.innerHTML =
      '<p class="empty-text">No connections found</p>';
  }
}

// ========== UTILITY FUNCTIONS ==========
function showError(message) {
  // Create error notification
  const notification = document.createElement("div");
  notification.className = "notification error";
  notification.innerHTML = `
        <i class="fas fa-exclamation-circle"></i>
        <span>${message}</span>
        <button onclick="this.parentElement.remove()">&times;</button>
    `;

  notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        background: #f44336;
        color: white;
        padding: 1rem 1.5rem;
        border-radius: 8px;
        display: flex;
        align-items: center;
        gap: 10px;
        z-index: 10000;
        box-shadow: 0 4px 12px rgba(244, 67, 54, 0.3);
        animation: slideIn 0.3s ease;
    `;

  document.body.appendChild(notification);

  // Auto remove after 5 seconds
  setTimeout(() => {
    if (notification.parentElement) {
      notification.remove();
    }
  }, 5000);
}

function showSuccess(message) {
  // Create success notification
  const notification = document.createElement("div");
  notification.className = "notification success";
  notification.innerHTML = `
        <i class="fas fa-check-circle"></i>
        <span>${message}</span>
        <button onclick="this.parentElement.remove()">&times;</button>
    `;

  notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        background: #4CAF50;
        color: white;
        padding: 1rem 1.5rem;
        border-radius: 8px;
        display: flex;
        align-items: center;
        gap: 10px;
        z-index: 10000;
        box-shadow: 0 4px 12px rgba(76, 175, 80, 0.3);
        animation: slideIn 0.3s ease;
    `;

  document.body.appendChild(notification);

  // Auto remove after 5 seconds
  setTimeout(() => {
    if (notification.parentElement) {
      notification.remove();
    }
  }, 5000);
}

// Add CSS for notifications
const style = document.createElement("style");
style.textContent = `
    @keyframes slideIn {
        from {
            transform: translateX(100%);
            opacity: 0;
        }
        to {
            transform: translateX(0);
            opacity: 1;
        }
    }
    
    .notification button {
        background: none;
        border: none;
        color: white;
        font-size: 1.5rem;
        cursor: pointer;
        padding: 0 0.5rem;
    }
    
    .empty-text {
        color: #666;
        text-align: center;
        padding: 2rem;
    }
`;
document.head.appendChild(style);
