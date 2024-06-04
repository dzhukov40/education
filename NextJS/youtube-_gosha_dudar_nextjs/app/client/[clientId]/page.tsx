

export default function ClientId({ params }: { params: { clientId: string } }) {
  return (
    <div>
        <h1>ClientId page</h1>
        <p>{params.clientId}</p>
    </div>
  );
}
