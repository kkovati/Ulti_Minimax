document.getElementById('player').classList.add('active');

function moveToBottom(card) {
    const activeHand = document.querySelector('.hand.active'); // Get the currently active hand

    // If the card has no original placeholder reference, store its original parent
    if (!card.dataset.originalParent) {
        card.dataset.originalParent = card.parentElement.dataset.id; // Store the original placeholder's unique ID
    }

    // Check if the card is in its original placeholder
    if (card.parentElement.dataset.id === card.dataset.originalParent) {
        // Move to active hand
        if (activeHand) {
            activeHand.appendChild(card);
        }
    } else {
        // Move back to the original placeholder using the stored ID
        const originalPlaceholder = document.querySelector(`[data-id="${card.dataset.originalParent}"]`);
        if (originalPlaceholder) {
            originalPlaceholder.appendChild(card);
        }
    }
}

document.querySelectorAll('.hand').forEach(hand => {
    hand.addEventListener('click', () => {
        // Reset all hands to yellow
        document.querySelectorAll('.hand').forEach(f => f.classList.remove('active'));
        // Set the clicked hand to white
        hand.classList.add('active');
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
