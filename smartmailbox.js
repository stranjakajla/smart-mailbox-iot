// Firebase konfiguracija
const firebaseConfig = {
    apiKey: "AIzaSyAe9qOZlkHtTR1KwYqigotvi7GkAMbaMTA",
    authDomain: "smart-mailbox-c4b1f.firebaseapp.com",
    databaseURL: "https://smart-mailbox-c4b1f-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "smart-mailbox-c4b1f",
    storageBucket: "smart-mailbox-c4b1f.firebasestorage.app",
    messagingSenderId: "203733054729",
    appId: "1:203733054729:web:67e7960ff1edb86ac04fd8"
};

// Inicijalizacija Firebase-a
firebase.initializeApp(firebaseConfig);
const database = firebase.database();
const eventsRef = database.ref('events');

// Funkcija za dohvaćanje podataka u realnom vremenu (ispravno sortiranje)
function displayMessages(filterType = null) {
    eventsRef.orderByChild('timestamp').on('value', (snapshot) => {
        const events = snapshot.val();
        const eventList = document.getElementById('event-list');
        const totalEvents = document.getElementById('total-events');
        const latestNotification = document.getElementById('latest-notification');

        eventList.innerHTML = ''; // Očisti listu
        let allEvents = [];

        if (!events) {
            totalEvents.textContent = `Ukupno događaja: 0`;
            eventList.innerHTML = "<p>Nema zabilježenih događaja.</p>";
            latestNotification.innerHTML = "<p><strong>Nema novih notifikacija.</strong></p>";
            return;
        }

        // Iteracija kroz sve događaje
        Object.keys(events).forEach(eventKey => {
            Object.values(events[eventKey]).forEach(eventData => {
                if (!filterType || eventData.type === filterType) {
                    allEvents.push(eventData);
                }
            });
        });

        //  Sortiranje prema `timestamp` (najnoviji događaji idu prvi)
        allEvents.sort((a, b) => b.timestamp - a.timestamp);

        // Ažuriranje najnovije notifikacije
        if (allEvents.length > 0) {
            const latestEvent = allEvents[0];
            latestNotification.innerHTML = `
                <p><strong>Vrijeme:</strong> ${latestEvent.time || "Nepoznato"}</p>
                <p><strong>Tip događaja:</strong> ${latestEvent.type || "Nepoznato"}</p>
                <p><strong>Poruka:</strong> ${latestEvent.message || "Nepoznato"}</p>
            `;
        } else {
            latestNotification.innerHTML = "<p><strong>Nema novih notifikacija.</strong></p>";
        }

        // Prikaz događaja na stranici (uvijek sortirani)
        allEvents.forEach(eventData => {
            const eventItem = document.createElement('div');
            eventItem.className = 'event-item';
            eventItem.innerHTML = `
                <p><strong>Vrijeme:</strong> ${eventData.time || "Nepoznato"}</p>
                <p><strong>Tip događaja:</strong> ${eventData.type || "Nepoznato"}</p>
                <p><strong>Poruka:</strong> ${eventData.message || "Nepoznato"}</p>
            `;
            eventList.appendChild(eventItem);
        });

        totalEvents.textContent = `Ukupno događaja: ${allEvents.length}`;
    });
}

// Event listeneri za filtriranje događaja (ostaje realtime)
document.getElementById('show-all').addEventListener('click', () => {
    displayMessages();
});

document.getElementById('show-owner').addEventListener('click', () => {
    displayMessages('Vlasnik prepoznat');
});

document.getElementById('show-postman').addEventListener('click', () => {
    displayMessages('Poštar prepoznat');
});

document.getElementById('show-danger').addEventListener('click', () => {
    displayMessages('Opasnost');
});

// Inicijalno pokretanje u realnom vremenu
displayMessages();


