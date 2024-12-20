// Generate the deck of cards
document.addEventListener('DOMContentLoaded', () => {
    const deck = document.querySelector('.deck');
    
    const nSuit = 4;
    const nCardPerSuit = 8;	
	cardSuitDiv = null;

    for (let iSuit = 0; iSuit < nSuit; iSuit++) {		
		// To have only two rows of cards in the deck
		if (iSuit % 2 == 0) {
			cardSuitDiv = document.createElement('div');
			cardSuitDiv.classList.add('card_suit');
		}
        
        for (let iCard = 0; iCard < nCardPerSuit; iCard++) {
			// Card placeholder to hold a fix position for the card in the deck
            const cardPlaceholderDiv = document.createElement('div');
            cardPlaceholderDiv.classList.add('card_placeholder');
            cardPlaceholderDiv.setAttribute('data-id', `placeholder${iSuit * nCardPerSuit + iCard}`);
            
			// Card
            const cardImage = document.createElement('img');
            cardImage.classList.add('card');
            cardImage.setAttribute('id', `${iSuit}${iCard}`);
            cardImage.setAttribute('src', `assets/images/${iSuit}${iCard}.jpg`);
            cardImage.setAttribute('alt', `Card ${iSuit}${iCard}`);
            cardImage.setAttribute('onclick', 'moveCard(this)');
            
            cardPlaceholderDiv.appendChild(cardImage);
            cardSuitDiv.appendChild(cardPlaceholderDiv);
        }
        
        deck.appendChild(cardSuitDiv);
    }
});


// Move card between deck and hands
function moveCard(card) {
    const activeHand = document.querySelector('.hand.active'); // Get the currently active hand

    // If the card has no original placeholder reference, store its original parent
    if (!card.dataset.originalParent) {
        card.dataset.originalParent = card.parentElement.dataset.id; // Store the original placeholder's unique ID
    }

    // Check if the card is in its original placeholder
    if (card.parentElement.dataset.id === card.dataset.originalParent) {
        // Move to active hand
		// Note: the limit is set to 11 instead of 10, because the hand already contains a background text
        if (activeHand && activeHand.children.length < 11) {
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


//TODO make trump active 

document.querySelectorAll('.trump').forEach(trump => {
    trump.addEventListener('click', () => {
        // Reset all images to default
        document.querySelectorAll('.trump').forEach(img => img.classList.remove('active'));
        // Set the clicked trump to white border
        trump.classList.add('active');
    });
});


// Assemble message to WASM by coding the hands into a string
function getDeal() {
    const hands = ['player', 'opponent_0', 'opponent_1'];
    let deal = "";

    hands.forEach(divId => {
        const hand = document.getElementById(divId);
        if (hand) {
            // Get all cards from hand, which are child elements of the div
            const cards = hand.children;

            // Iterate over children and collect their ids
            for (let card of cards) {
                if (card.id) {  // Only collect if the child has an id
                    deal += card.id;
                }
            }
        }
    });

    return deal;
}


// Toast warning message
function showToast(message) {
	var toast = document.getElementById("toast");
	toast.textContent = message;
	toast.style.visibility = "visible";
	setTimeout(function() {
		toast.style.visibility = "hidden";
	}, 3000); // Toast will disappear after 3 seconds
}


// Simulate button opens simulation side and codes deal into URL
document.getElementById("simulate-button").addEventListener("click", () => {
	const deal = getDeal();
	
	// Check length
	if (deal.length != 60) {
		showToast("Deal 10 cards to each player");
		return;
	}
	
	window.location.href = `simulation.html?deal=${deal}`;
});