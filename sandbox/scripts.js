function moveToBottom(card) {
    const activeHand = document.querySelector('.hand.active');
    const deck = document.querySelector('.deck');

    if (activeHand && !activeHand.contains(card)) {
        activeHand.appendChild(card);
    } else {
        deck.appendChild(card);
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
