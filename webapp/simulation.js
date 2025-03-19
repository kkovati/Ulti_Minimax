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

// Initialize
document.addEventListener('DOMContentLoaded', () => {
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
});

// Put cards onto game-progression div based on game progression string
function showGameProgression(gameProgression) {
	console.assert(gameProgression.length == 91)
	const result = gameProgression[0];
	const gp = gameProgression.substring(1, 91);
	
	// Delete previous game progression displayed	
	document.getElementById(`game-progression`).innerHTML = "";    
	
	// Display parameters
	const nRow = 2; 
	const nRoundInRow = 5;
	console.assert(nRow * nRoundInRow == 10);
	const offsetX = 40; // Offset from parent div's edge
	const offsetY = 130;	
	const playerOffsetX = 30; // Player name offset from parent div's edge
	const playerOffsetY = 10;
	const nextPlayerOffsetX = 40; // Next player name offset from previous
	const nextPlayerOffsetY = 30;
	const indexOffsetX = -25; // Round index text offset from round
	const indexOffsetY = 0;
	const roundWidht = 210; // Width of 3 cards in a round
	const roundHeight = 180;	
	const stepInRoundX = 50; // Vertical step of cards in a round
	const stepInRoundY = 20;
	
	// Display cards and texts
	setTimeout(() => { // Blink effect
		// Display player texts
		["Player", "Opponent 1", "Opponent 2"].forEach((item, index) => {
			const playerName = document.createElement("h1");
			playerName.className = "main-text";
			playerName.textContent = item;
			playerName.style.position = "absolute";
			playerName.style.left = `${playerOffsetX + index * nextPlayerOffsetX}px`;
			playerName.style.top = `${playerOffsetY + index * nextPlayerOffsetY}px`;
			document.getElementById(`game-progression`).appendChild(playerName);
		});
	
		// Iterate through rounds
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
				
				// Display round index text
				const roundIndex = document.createElement("h1");
				roundIndex.className = "main-text";
				roundIndex.textContent = `${iRound + 1}`;
				roundIndex.style.position = "absolute";
				roundIndex.style.left = `${roundX + indexOffsetX - (iRound == 9 ? 5 : 0)}px`;
				roundIndex.style.top = `${roundY + indexOffsetY}px`;
				document.getElementById(`game-progression`).appendChild(roundIndex);
				
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
	}, 200); // Short delay
}


// Initialize WASM code, simulation management and start the simulation
function initializeSimulation() {
	// Get the button element
	const button = document.getElementById("simulate-button");
	if (!button) {
        console.error("Button not found");
        return;
    }

	// Add a click event listener to trigger WASM code on button click
	button.addEventListener("click", async () => {
		
		// Reset simulation statuses
		document.querySelectorAll('.game-type').forEach(gameTypeDiv => {
			gameTypeDiv.querySelector('.status').innerHTML = "";
			gameTypeDiv.querySelectorAll("button").forEach(button_ => button_.remove());
		});
		
		// Delete previous game progression displayed	
		document.getElementById(`game-progression`).innerHTML = "";

		// Ensure the browser processes the DOM update and starts the animation
		await new Promise(resolve => requestAnimationFrame(() => resolve()));
		
		// Iterate through all game types
		for (const gameTypeDiv of document.querySelectorAll('.game-type')) {

			// Get game type
			const gameType = gameTypeDiv.dataset.index;
			
			// Add the spinning loader
			const statusSpan = gameTypeDiv.querySelector('.status');
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
				gameTypeDiv.insertBefore(show_button, gameTypeDiv.querySelector(".status").nextSibling);
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
}

// Wait for both DOM and WASM module to be loaded, then run initializeSimulation()
Promise.all([
    new Promise(resolve => {
        if (document.readyState === "complete" || document.readyState === "interactive") {
            resolve();
        } else {
            document.addEventListener("DOMContentLoaded", resolve);
        }
    }),
    new Promise(resolve => {
        Module.onRuntimeInitialized = resolve;
    })
]).then(() => {
	console.log("DOM and WASM module is loaded");
    initializeSimulation();
});
