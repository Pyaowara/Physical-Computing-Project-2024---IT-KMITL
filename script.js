const blynkApiUrl = "https://sgp1.blynk.cloud/external/api/get?token=MVdAnf5wPMmTnlj1_lAReSsrLohc_f9h&";
const topics = {
    co: { name: 'CO', units: 'ppm', virtualPort: 'v1' },
    co2: { name: 'CO2', units: 'ppm', virtualPort: 'v3' },
    alcohol: { name: 'Alcohol', units: 'ppm', virtualPort: 'v2' },
    nh4: { name: 'NH4', units: 'ppm', virtualPort: 'v5' },
    airQuality: { name: 'Air Quality', units: '%', virtualPort: 'v7' },
};

const gaugesContainer = document.getElementById('gaugesContainer');
const airQualityValueElement = document.getElementById('skibidi/airquality-value');

function createGaugeElement(topicData) {
    const cardDiv = document.createElement('div');
    cardDiv.className = 'col-md-3 mb-4';
    const card = `
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">${topicData.name}</h5>
                <div class="gauge-container">
                    <span class="gauge-value" id="${topicData.virtualPort}-value">--</span> <span class="units">${topicData.units}</span>
                </div>
            </div>
        </div>
    `;

    cardDiv.innerHTML = card;
    gaugesContainer.appendChild(cardDiv);
}

async function fetchBlynkData() {
    for (const key in topics) {
        const apiUrl = `${blynkApiUrl}${topics[key].virtualPort}`;
        try {
            const response = await fetch(apiUrl);
            const data = await response.text();


            if (key === 'airQuality' && airQualityValueElement) {
                airQualityValueElement.textContent = data;
            } else {
                const gaugeValueElement = document.getElementById(`${topics[key].virtualPort}-value`);
                if (gaugeValueElement) {
                    gaugeValueElement.textContent = data;
                }}
        } catch (error) {
            console.error(`Error fetching data for ${topics[key].name}:`, error);
        }
    }
}

for (const key in topics) {
    if (key !== 'airQuality') {
        createGaugeElement(topics[key]);
    }
}

setInterval(fetchBlynkData, 1000);
