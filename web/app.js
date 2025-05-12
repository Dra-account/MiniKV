function sendQuery() {
    const sql = document.getElementById("sqlInput").value;
    fetch('/api/query', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain'
        },
        body: sql
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById("resultArea").textContent = data;
    })
    .catch(error => {
        document.getElementById("resultArea").textContent = `错误: ${error}`;
    });
}
