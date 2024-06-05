import Post from "@/app/components/Post";

async function fetchData(id: number) {
  const res = await fetch('https://jsonplaceholder.typicode.com/posts/' + id);
  const result = await res.json();
  return result;
}


const PagePost = async ({ params }: { params: { id: number } }) => {
  const currentPost = await fetchData(params.id);
   
    return (
      <Post id={currentPost.id} title={currentPost.title} body={currentPost.body} />
    )
}

export default PagePost
