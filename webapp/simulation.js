// Get deal string from URL
const urlParams = new URLSearchParams(window.location.search);
var base_deal = urlParams.get('deal');

// Check deal string
if (!base_deal) {
	throw new Error("No deal string found");
}
if (base_deal.length != 61) {
	throw new Error("Deal string length is not correct");
}
console.log("Deal from URL: " + base_deal);

// Trim deal string and get trump index
var trumpIndex = base_deal[0];
base_deal = base_deal.substring(1);
console.assert(base_deal.length == 60)

// Make given trump active initially
document.getElementById(`trump-${trumpIndex}`).classList.add('active');

// Make trump active if clicked
document.querySelectorAll('.trump').forEach(trump => {
    trump.addEventListener('click', () => {
        // Reset all images to default
        document.querySelectorAll('.trump').forEach(img => img.classList.remove('active'));
        // Set the clicked trump to white border
        trump.classList.add('active');
		// Set trump index
		trumpIndex = trump.dataset.index;
    });
});

// Put cards onto game-progression div based on game progression string
function showGameProgression(gameProgression) {
	console.assert(gameProgression.length == 91)
	const result = gameProgression[0];
	const gp = gameProgression.substring(1, 91);
	
	// Display parameters
	const nRow = 2; 
	const nRoundInRow = 5;
	console.assert(nRow * nRoundInRow == 10);
	const offsetX = 10; // Offset from parent div's edge
	const offsetY = 10;	
	const roundWidht = 200; // Width of 3 cards in a round
	const roundHeight = 150;	
	const stepInRoundX = 50; // Vertical step of cards in a round
	const stepInRoundY = 20;
	
	// Display cards
	for (let iRow = 0; iRow < nRow; iRow++) {
		for (let iRoundInRow = 0; iRoundInRow < nRoundInRow; iRoundInRow++) {
			let iRound = iRow * nRoundInRow + iRoundInRow;
			let startIdx = iRound * 9; // Start index of round in gameProgression
			
			if (gp[startIdx] > 2 || gp[startIdx + 3] > 2 || gp[startIdx + 6] > 2) {
				break; // Invalid round, stop displaying more cards
			}
			
			// Coords of the round
			let roundX = offsetX + iRoundInRow * roundWidht;
			let roundY = offsetY + iRow * roundHeight;
			
			// Coords of the 3 cards in a round
			for (let iCard = 0; iCard < 3; iCard++) {
				// X depends on which player's cards
				let cardX = roundX + gp[startIdx + iCard * 3] * stepInRoundX;
				// Y depends on which order the cards were hit
				let cardY = roundY + iCard * stepInRoundY;
				
				// Create card
				let suit = gp[startIdx + iCard * 3 + 1];
				let value = gp[startIdx + iCard * 3 + 2];
				const cardImage = document.createElement('img');
				cardImage.classList.add('card');
				cardImage.setAttribute('src', `assets/images/${suit}${value}.jpg`);
				cardImage.setAttribute('alt', `Card ${suit}${value}`);
				cardImage.style.position = "absolute";
				cardImage.style.left = `${cardX}px`;
				cardImage.style.top = `${cardY}px`;
				
				// Display card
				document.getElementById(`game-progression`).appendChild(cardImage);
			}
		}
	}
}

// This ensures the WASM code will run once it's loaded
Module.onRuntimeInitialized = () => {
	console.log("WASM module loaded");

	// Get the button element
	const button = document.getElementById("simulate-button");

	// Add a click event listener to trigger WASM code on button click
	button.addEventListener("click", async () => {
		
		// Reset simulation statuses
		document.querySelectorAll('.game-type').forEach(gameTypeSpan => {
			gameTypeSpan.querySelector('.status').innerHTML = "";
		});
		
		// Ensure the browser processes the DOM update and starts the animation
		await new Promise(resolve => requestAnimationFrame(() => resolve()));
		
		// Iterate through all game types
		for (const gameTypeSpan of document.querySelectorAll('.game-type')) {

			// Get game type
			const gameType = gameTypeSpan.dataset.index;
			
			// Add the spinning loader
			const statusSpan = gameTypeSpan.querySelector('.status');
			statusSpan.innerHTML = '<div class="loading"></div>';
			
			// Ensure the browser processes the DOM update and starts the animation
			await new Promise(resolve => requestAnimationFrame(() => resolve()));
			
			// Deal code structure:
			// deal[0]		= game type
			// deal[1]		= trump [0-3]
			// deal[2...61]	= cards
			const deal = gameType + trumpIndex + base_deal;
			console.assert(deal.length == 62)
			
			// Allocate memory for deal string in WASM
			const length = deal.length + 1;
			const dealPointer = Module._malloc(length);
			Module.stringToUTF8(deal, dealPointer, length);

			// Call the WASM entry point
			const gameProgressionPointer = Module._wasm_main(dealPointer);

			// Read game progression string
			// gameProgression[0] = result 
			// gameProgression[1] = round starting player index 
			// gameProgression[2] = card suit 
			// gameProgression[3] = card value
			// gameProgression[4] = next player index in the round
			const gameProgression = Module.UTF8ToString(gameProgressionPointer);
			console.assert(gameProgression.length == 91)
			console.log("WASM returned:", gameProgression);

			// Free the memory allocated in WASM (C++)
			Module._free(dealPointer);
			Module._free(gameProgressionPointer);
			
			// Update status based on game result
			const result = gameProgression[0];
			if (result == "1") {
				// Player win
				statusSpan.innerHTML = '✔';
				
				// Button to show game progression
				let show_button = document.createElement("button");
				show_button.textContent = "Show";
				show_button.classList.add("green-button");
				show_button.classList.add("smaller");
				//show_button.dataset.index = index;
				//show_button.dataset.info = gameProgression;
				show_button.addEventListener("click", () => showGameProgression(gameProgression));
				gameTypeSpan.insertBefore(show_button, gameTypeSpan.querySelector(".status").nextSibling);
			}
			else if (result == "2") {
				// Opponent win
				statusSpan.innerHTML = '✘';
			}
			else {
				throw new Error('Undefined result was returned from simulation');
			}			
			
		}

	});
	
	// Trigger simulation by clicking the button
	button.click();
};