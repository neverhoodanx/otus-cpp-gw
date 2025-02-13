build:
    docker build -f Dockerfile.build -t server-builder .
    docker build -f Dockerfile.run -t server-run .

run:
    docker run -d --name message_service -p 9090:9090 server-run

logs:
    docker logs message_service

clean:
    docker rm -f message_service || true
    docker rmi server-builder server-run || true