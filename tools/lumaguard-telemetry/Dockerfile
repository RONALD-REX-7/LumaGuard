FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1
WORKDIR /opt/lumaguard-telemetry
COPY . .
RUN python -m pip install --no-cache-dir . \
    && useradd --create-home --uid 10001 appuser
USER appuser
ENTRYPOINT ["lumaguard-telemetry"]
CMD ["--help"]
