function sendPostForm(event) {
  event.preventDefault();
  const filename = document.getElementById('post-filename').value;
  const message = document.getElementById('post-message').value;

  console.log("Salut");
  if (!filename) {
    return;
  }

  fetch(`/upload/${encodeURIComponent(filename)}`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ message: message })
  }).then(response => {
  }).catch(error => {
    console.error(error);
  });
}

function sendDeleteForm(event) {
  event.preventDefault();
  const filename = document.getElementById('delete-filename').value;

  if (!filename) {
    return;
  }

  fetch(`/upload/${encodeURIComponent(filename)}`, {
    method: 'DELETE'
  }).then(response => {
  }).catch(error => {
    console.error(error);
  });
}