function sendDeleteForm(event) {
    event.preventDefault();
    const filename = document.getElementById('delete-filename').value.trim();
    if (!filename) return;

    fetch(`/upload/${encodeURIComponent(filename)}`, {
      method: 'DELETE',
      headers: { 'Accept': '*/*' }
    });
  }

async function sendUploadForm(event) {
  event.preventDefault();
  const fileInput = document.getElementById('fileInput');
  const file = fileInput.files[0];
  if (!file) {
    console.log("Aucun fichier sélectionné");
    return;
  }

  // On envoie le fichier en PUT directement sur /upload/<filename>
  const res = await fetch(`/upload/${encodeURIComponent(file.name)}`, {
    method: 'POST',
    headers: {
      'Content-Type': file.type || 'application/octet-stream'
    },
    body: file
  });

  if (res.ok) {
    console.log(`✅ ${file.name} uploadé`);
    dropZone.textContent = "Glissez un fichier ici ou cliquez pour le sélectionner";
    fileInput.value = '';
  } else {
    console.error(`❌ Échec de l'upload: ${res.status}`);
  }
}

// DRAG & DROP reste identique
const dropZone = document.getElementById('drop-zone');
const fileInput = document.getElementById('fileInput');

dropZone.addEventListener('click', () => fileInput.click());
dropZone.addEventListener('dragover', e => { e.preventDefault(); dropZone.classList.add('dragover'); });
dropZone.addEventListener('dragleave', () => dropZone.classList.remove('dragover'));
dropZone.addEventListener('drop', e => {
  e.preventDefault();
  dropZone.classList.remove('dragover');
  const file = e.dataTransfer.files[0];
  if (file) {
    fileInput.files = e.dataTransfer.files;
    dropZone.textContent = file.name;
  }
});
fileInput.addEventListener('change', () => {
  const file = fileInput.files[0];
  dropZone.textContent = file ? file.name : "Glissez un fichier ici ou cliquez pour le sélectionner";
});