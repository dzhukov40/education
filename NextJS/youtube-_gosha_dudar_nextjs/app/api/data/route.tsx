

export async function GET(request:any) {
    const data = {
        name: 'Bob',
        age: 10,
        bio: 'Some info'
    }

    return new Response(JSON.stringify(data));
}

export async function POST(request:any) {
    return new Response(request.body);
}

