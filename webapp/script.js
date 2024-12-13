function moveToBottom(card) {
    const bottomContainer = document.getElementById("bottomContainer");
    card.style.transition = "none";
    card.style.transform = "none";
    bottomContainer.appendChild(card);
}
