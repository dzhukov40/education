import Link from "next/link"

const Post = (post : { id: number, title: string, body: string }) => {
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
