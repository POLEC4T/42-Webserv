function getCookie(name) {
    const cookies = document.cookie.split("; ");
    for (const cookie of cookies) {
        const [key, value] = cookie.split("=");
        if (key === name) return value;
    }
    return null;
}

function setCookie(name, value, path = "/") {
    document.cookie = `${name}=${value}; path=${path}`;
}

function generateRandomId(length = 6) {
    const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    let result = "";
    for (let i = 0; i < length; i++) {
        result += chars.charAt(Math.floor(Math.random() * chars.length));
    }
    return result;
}

// --- Afficher le profil ---
function renderProfile() {
    const container = document.getElementById("profile-content");
    let id = getCookie("id");

    if (!id) {
        id = generateRandomId();
        setCookie("id", id);
    }

    container.innerHTML = `
      <h3 class="text-lg text-gray-200 mb-2">Player_${id}</h3>
      <p class="text-gray-500 text-sm mb-4">Niveau 12 — Explorateur de mondes</p>
      <div class="xp-bar bg-gray-700 rounded-full overflow-hidden h-3">
        <div class="xp-fill bg-green-500 h-3" style="width:70%"></div>
      </div>
      <p class="text-xs text-gray-500 mt-2">70% vers le prochain niveau</p>
      <div class="mt-6">
        <button id="change-id" class="btn text-sm">Modifier le profil</button>
      </div>
    `;

    document.getElementById("change-id").addEventListener("click", () => {
        const newId = generateRandomId();
        setCookie("id", newId);
        location.reload();
    });
}

window.addEventListener("load", renderProfile);