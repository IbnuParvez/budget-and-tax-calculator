
FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN make clean && make


FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libmysqlclient21 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /src/budget_tax_system .

ENTRYPOINT ["./budget_tax_system"]
