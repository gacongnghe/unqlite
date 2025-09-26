#!/bin/bash

# =============================================================================
# Gnosis Safe Database Initialization Script
# =============================================================================
# This script initializes the database and creates necessary users

set -e

echo "🚀 Initializing Gnosis Safe database..."

# Wait for PostgreSQL to be ready
echo "⏳ Waiting for PostgreSQL to be ready..."
until docker-compose exec -T postgres pg_isready -U ${POSTGRES_USER:-safe_user}; do
  echo "PostgreSQL is unavailable - sleeping"
  sleep 2
done
echo "✅ PostgreSQL is ready!"

# Run migrations for transaction service
echo "📦 Running migrations for transaction service..."
docker-compose exec -T transaction-service python manage.py migrate

# Run migrations for config service
echo "📦 Running migrations for config service..."
docker-compose exec -T config-service python src/manage.py migrate

# Create superuser for transaction service if it doesn't exist
echo "👤 Creating superuser for transaction service..."
if ! docker-compose exec -T transaction-service python manage.py shell -c "from django.contrib.auth import get_user_model; User = get_user_model(); User.objects.filter(username='admin').exists()" | grep -q "True"; then
    echo "from django.contrib.auth import get_user_model; User = get_user_model(); User.objects.create_superuser('admin', 'admin@example.com', 'admin123')" | docker-compose exec -T transaction-service python manage.py shell
    echo "✅ Admin user created for transaction service (username: admin, password: admin123)"
else
    echo "ℹ️  Admin user already exists for transaction service"
fi

# Create superuser for config service if it doesn't exist
echo "👤 Creating superuser for config service..."
if ! docker-compose exec -T config-service python src/manage.py shell -c "from django.contrib.auth import get_user_model; User = get_user_model(); User.objects.filter(username='admin').exists()" | grep -q "True"; then
    echo "from django.contrib.auth import get_user_model; User = get_user_model(); User.objects.create_superuser('admin', 'admin@example.com', 'admin123')" | docker-compose exec -T config-service python src/manage.py shell
    echo "✅ Admin user created for config service (username: admin, password: admin123)"
else
    echo "ℹ️  Admin user already exists for config service"
fi

echo "🎉 Database initialization completed!"
echo ""
echo "📋 Admin credentials:"
echo "  - Transaction Service: http://localhost:8000/admin (admin/admin123)"
echo "  - Config Service: http://localhost:8001/admin (admin/admin123)"
echo ""
echo "⚠️  IMPORTANT: Change default passwords in production!"