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
	button.addEventListener("click", () => {
		
		// TODO reset simulation statuses
		
		// Deal code structure:
		// deal[0]		= game type
		// deal[1]		= trump [0-3]
		// deal[2...61]	= cards
		var gameType = "0";
		const deal = gameType + trumpIndex + base_deal;
		console.assert(deal.length == 62)
		
		const length = deal.length + 1;
		const dealPointer = Module._malloc(length);
		Module.stringToUTF8(deal, dealPointer, length);

		// Call the WASM entry point
		const resultPointer = Module._wasm_main(dealPointer);

		// Read the result string
		const resultString = Module.UTF8ToString(resultPointer);
		console.log("WASM returned:", resultString);

		// Free the allocated memory in both JS and WASM
		Module._free(dealPointer);
		Module._free(resultPointer); // Free the memory allocated in C++ for the response
	});
	
	// Trigger simulation by clicking the button
	button.click();
};