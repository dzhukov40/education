'use client';

import { useEffect } from 'react';
import dynamic from 'next/dynamic';
import 'leaflet/dist/leaflet.css';

const LeafletMap = () => {
    useEffect(() => {
        const L = require('leaflet');

        const mapContainer = document.getElementById('map') as HTMLElement;
        
        // Ensure the map container is not null and not already initialized
        if (mapContainer && !mapContainer.hasOwnProperty('_leaflet_id')) {
            const map = L.map('map').setView([52.52, 13.405], 13);

            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
            }).addTo(map);

            L.marker([52.52, 13.405]).addTo(map)
                .bindPopup('Berlin')
                .openPopup();
        }
    }, []);

    return <div id="map" style={{ height: '600px', width: '100%' }}></div>;
};

export default dynamic(() => Promise.resolve(LeafletMap), { ssr: false });
