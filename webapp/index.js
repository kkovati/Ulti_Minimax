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
            const cardPlaceholderDiv = document.createElement('div');
            cardPlaceholderDiv.classList.add('card_placeholder');
            cardPlaceholderDiv.setAttribute('data-id', `placeholder${iSuit * nCardPerSuit + iCard}`);
            
            const cardImage = document.createElement('img');
            cardImage.classList.add('card');
            cardImage.setAttribute('id', `card${iSuit * nCardPerSuit + iCard}`);
            //cardImage.setAttribute('src', `path/to/card${iSuit * nCardPerSuit + iCard}.png`);
            cardImage.setAttribute('src', `assets/images/${iSuit}${iCard}.jpg`);
            cardImage.setAttribute('alt', `Card ${iSuit * nCardPerSuit + iCard}`);
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
        if (activeHand && activeHand.children.length < 10) {
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

document.querySelectorAll('.trump').forEach(trump => {
    trump.addEventListener('click', () => {
        // Reset all images to default
        document.querySelectorAll('.trump').forEach(img => img.classList.remove('active'));
        // Set the clicked trump to white border
        trump.classList.add('active');
    });
});


// Wait for the WASM module to load
//Module.onRuntimeInitialized = () => {
//    // Access the 'add' function from the WASM module
//    const add = Module.cwrap('add', 'number', ['number', 'number']);
//
//    document.getElementById('compute').addEventListener('click', () => {
//        const result = add(5, 3); // Call the WASM function
//        document.getElementById('result').textContent = `Result: ${result}`;
//    });
//};


// Wait for the WASM module to load
//Module.onRuntimeInitialized = () => {
//    // Access the 'multiply' function from the WASM module
//    const multiply = Module.cwrap('multiply', 'number', ['number', 'number']);
//
//    // Add event listener for button
//    document.getElementById('calculate').addEventListener('click', () => {
//        const num1 = parseInt(document.getElementById('num1').value, 10);
//        const num2 = parseInt(document.getElementById('num2').value, 10);
//        if (!isNaN(num1) && !isNaN(num2)) {
//            const result = multiply(num1, num2); // Call the WASM function
//            document.getElementById('result').textContent = `Result: ${result}`;
//        } else {
//            document.getElementById('result').textContent = 'Please enter valid numbers!';
//        }
//    });
//};


// Load the WASM module
//Module.onRuntimeInitialized = () => {
//	console.log("WASM module loaded");
//    const deal = "071315172021303336370001030405111214223202061016232426273134";
//    const length = deal.length + 1;
//    const dealPointer = Module._malloc(length);
//    Module.stringToUTF8(deal, dealPointer, length);
//
//	// Call the WASM entry point
//    const resultPointer = Module._wasm_main(dealPointer);
//	
//	// Read the result string
//    const resultString = Module.UTF8ToString(resultPointer);
//    console.log("WASM returned:", resultString);
//
//	// Free the allocated memory in both JS and WASM
//    Module._free(dealPointer);
//	Module._free(resultPointer); // Free the memory allocated in C++ for the response
//};