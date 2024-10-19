
const brokerUrl = 'wss://mqtt-dashboard.com:8884/mqtt';
const topics = {
    co: { topic: 'skibidi/co', name: 'CO', units: 'ppm', max: 40 },
    co2: { topic: 'skibidi/co2', name: 'CO2', units: 'ppm', max: 2000 },
    nh4: { topic: 'skibidi/nh4', name: 'NH4', units: 'ppm', max: 50 },
    alcohol: { topic: 'skibidi/alcohol', name: 'Alcohol', units: 'ppm', max: 100 },
};




const client = mqtt.connect(brokerUrl);
const gaugesContainer = document.getElementById('gaugesContainer');



function createGaugeElement(topicData) {
    const cardDiv = document.createElement('div');
    cardDiv.className = 'col-md-3 mb-4';
    const card = `
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">${topicData.name}</h5>
                <div class="gauge-container">
                    <span class="gauge-value" id="${topicData.topic}-value">--</span> <span class="units">${topicData.units}</span>
                </div>
            </div>
        </div>
    `;

    cardDiv.innerHTML = card;
    gaugesContainer.appendChild(cardDiv);
}


client.on('connect', () => {
    console.log('Connected to MQTT broker');
    for (const key in topics) {
      createGaugeElement(topics[key]);


        client.subscribe(topics[key].topic, (err) => {
            if (err) {
                console.error(`Failed to subscribe to ${topics[key].topic}:`, err);
            }
        });
    }

});




client.on('message', (topic, message) => {
    const value = message.toString();
    const gaugeValueElement = document.getElementById(`${topic}-value`); // Use template literal

    if (gaugeValueElement) {
        gaugeValueElement.textContent = value;
        if (topic === 'skibidi/airquality') {
            gaugeValueElement.classList.add('large-value');
        }
    
    }
});