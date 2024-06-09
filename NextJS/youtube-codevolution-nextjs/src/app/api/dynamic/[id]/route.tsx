export async function GET(
    request: Request,
    { params } : { 
        params: {
          id: string
        }
    }
) {

    return new Response("GET handler, id=" + params.id);
}

