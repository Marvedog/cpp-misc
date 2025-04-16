CREATE TABLE chat (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW(),
);

CREATE TABLE message (
    id SERIAL PRIMARY KEY,
    room_id INTEGER REFERENCES chat(id),
    user_id INTEGER REFERENCES users(id),
    content TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT NOW(),
);
