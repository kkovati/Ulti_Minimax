
const urlParams = new URLSearchParams(window.location.search);
const deal = urlParams.get('deal');

if (deal) {
	console.log(deal);
} else {
	// TODO handle this error
	document.getElementById('content').textContent = 'No data provided.';
}

//TODO make trump active from URL

document.querySelectorAll('.trump').forEach(trump => {
    trump.addEventListener('click', () => {
        // Reset all images to default
        document.querySelectorAll('.trump').forEach(img => img.classList.remove('active'));
        // Set the clicked trump to white border
        trump.classList.add('active');
    });
});


// This ensures the WASM code will run once it's loaded
Module.onRuntimeInitialized = () => {
	console.log("WASM module loaded");

	// Get the button element
	const button = document.getElementById("simulate-button");

	// Add a click event listener to trigger WASM code on button click
	button.addEventListener("click", () => {
		const deal = getDeal();
		
		// Check length
		if (deal.length != 60) {
			showToast("Deal is not ready!");
			return;
		}
		
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
};