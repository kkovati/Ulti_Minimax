function moveToBottom(card) {
    const activeFrame = document.querySelector('.frame.active');
    const deck = document.querySelector('.deck');

    if (activeFrame && !activeFrame.contains(card)) {
        activeFrame.appendChild(card);
    } else {
        deck.appendChild(card);
    }
}

document.querySelectorAll('.frame').forEach(frame => {
    frame.addEventListener('click', () => {
        // Reset all frames to yellow
        document.querySelectorAll('.frame').forEach(f => f.classList.remove('active'));
        // Set the clicked frame to white
        frame.classList.add('active');
    });
});

document.querySelectorAll('.image').forEach(image => {
    image.addEventListener('click', () => {
        // Reset all images to default
        document.querySelectorAll('.image').forEach(img => img.classList.remove('active'));
        // Set the clicked image to white border
        image.classList.add('active');
    });
});
