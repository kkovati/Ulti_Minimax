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
			const resultPointer = Module._wasm_main(dealPointer);

			// Read game progression string
			// gameProgression[0] = result 
			// gameProgression[1] = round starting player index 
			// gameProgression[2] = card suit 
			// gameProgression[3] = card value
			// gameProgression[4] = next player index in the round
			const gameProgression = Module.UTF8ToString(resultPointer);
			console.assert(gameProgression.length == 91)
			console.log("WASM returned:", gameProgression);

			// Free the allocated memory in both JS and WASM
			Module._free(dealPointer);
			Module._free(resultPointer); // Free the memory allocated in C++ for the response
			
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
				//show_button.dataset.index = index; // Store index for reference if needed
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