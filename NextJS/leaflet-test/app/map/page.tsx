// app/page.tsx
import Head from 'next/head';
import LeafletMap from '../components/LeafletMap';

export default function Home() {
    return (
        <div>
            <Head>
                <title>Leaflet Map Example</title>
                <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css" />
            </Head>
            <h1>Leaflet Map Centered on Berlin</h1>
            <LeafletMap />
        </div>
    );
}
