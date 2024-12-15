// scripts.js
document.querySelectorAll('.frame').forEach(frame => {
    frame.addEventListener('click', () => {
        // Reset all frames to yellow
        document.querySelectorAll('.frame').forEach(f => f.classList.remove('active'));
        // Set the clicked frame to white
        frame.classList.add('active');
    });
});
