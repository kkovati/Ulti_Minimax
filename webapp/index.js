function moveToBottom(card) {
    const bottomContainer = document.getElementById("bottomContainer");
    card.style.transition = "none";
    card.style.transform = "none";
    bottomContainer.appendChild(card);
}


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
Module.onRuntimeInitialized = () => {
    console.log("WASM module loaded");
    // Call the entry point
    Module._wasm_entrypoint();
};