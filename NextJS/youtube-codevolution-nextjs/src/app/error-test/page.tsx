

export default function ErrorTest() {
  const number = Math.random() * 2;
  
  if (number > 1) {
    throw new Error("Error loading");
  }

  return <h1>Test: {number}</h1>;
}
