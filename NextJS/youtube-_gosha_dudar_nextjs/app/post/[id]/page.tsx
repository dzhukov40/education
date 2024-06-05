import Link from "next/link";


async function fetchData(id: number) {
  const res = await fetch('https://jsonplaceholder.typicode.com/posts/' + id);
  const result = await res.json();
  return result;
}


const Post = async ({ params }: { params: { id: number } }) => {
  const post = await fetchData(params.id);
    return (
      <div>
        <Link href="/">Назад</Link><br />
        <h2>{post.title}</h2>
        <p>{post.body}</p>
        <strong>Avtor ID: {post.id}</strong>
      </div>
    )
}

export default Post
