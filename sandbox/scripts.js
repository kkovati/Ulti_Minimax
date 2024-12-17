document.addEventListener('DOMContentLoaded', () => {
    const deck = document.querySelector('.deck');
    
    const nSuit = 4;
    const nCardPerSuit = 8;

    for (let iSuit = 0; iSuit < nSuit; iSuit++) {
        const cardSuitDiv = document.createElement('div');
        cardSuitDiv.classList.add('card_suit');
        
        for (let iCard = 1; iCard <= nCardPerSuit; iCard++) {
            const cardPlaceholderDiv = document.createElement('div');
            cardPlaceholderDiv.classList.add('card_placeholder');
            cardPlaceholderDiv.setAttribute('data-id', `placeholder${iSuit * nCardPerSuit + iCard}`);
            
            const cardDiv = document.createElement('div');
            cardDiv.classList.add('card');
            cardDiv.textContent = `Card ${iSuit * nCardPerSuit + iCard}`;
            cardDiv.setAttribute('onclick', 'moveCard(this)'); // Dynamically attach the onclick handler
            
            cardPlaceholderDiv.appendChild(cardDiv);
            cardSuitDiv.appendChild(cardPlaceholderDiv);
        }
        
        deck.appendChild(cardSuitDiv);
    }
});

//document.querySelectorAll('.card').forEach(card => {
//    card.addEventListener('click', () => {
//        moveCard(card);
//    });
//});

function moveCard(card) {
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

// Make the player hand active initially
document.getElementById('player').classList.add('active');

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
