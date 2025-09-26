#!/bin/bash

# =============================================================================
# Gnosis Safe Health Check Script
# =============================================================================
# This script checks the health of all Gnosis Safe services

set -e

echo "🔍 Gnosis Safe Health Check"
echo "=========================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to check HTTP endpoint
check_endpoint() {
    local name=$1
    local url=$2
    local expected_status=${3:-200}
    
    echo -n "Checking $name... "
    
    if response=$(curl -s -o /dev/null -w "%{http_code}" --connect-timeout 5 "$url" 2>/dev/null); then
        if [ "$response" = "$expected_status" ]; then
            echo -e "${GREEN}✅ OK${NC} ($response)"
            return 0
        else
            echo -e "${YELLOW}⚠️  WARNING${NC} ($response)"
            return 1
        fi
    else
        echo -e "${RED}❌ ERROR${NC} (Connection failed)"
        return 1
    fi
}

# Function to check Docker container
check_container() {
    local container_name=$1
    local service_name=$2
    
    echo -n "Checking $service_name container... "
    
    if docker-compose ps "$container_name" | grep -q "Up"; then
        echo -e "${GREEN}✅ Running${NC}"
        return 0
    else
        echo -e "${RED}❌ Not running${NC}"
        return 1
    fi
}

echo ""
echo "🐳 Docker Containers:"
echo "-------------------"

check_container "postgres" "PostgreSQL"
check_container "redis" "Redis"
check_container "transaction-service" "Transaction Service"
check_container "config-service" "Config Service"
check_container "client-gateway" "Client Gateway"
check_container "safe-react" "Safe React"
check_container "nginx" "Nginx"

echo ""
echo "🌐 HTTP Endpoints:"
echo "----------------"

check_endpoint "Safe React Frontend" "http://localhost:3000"
check_endpoint "Transaction Service API" "http://localhost:8000/health"
check_endpoint "Config Service API" "http://localhost:8001/health"
check_endpoint "Client Gateway API" "http://localhost:8002/health"
check_endpoint "Nginx Proxy" "http://localhost/health"

echo ""
echo "📊 System Resources:"
echo "------------------"

# Check disk space
disk_usage=$(df -h / | awk 'NR==2 {print $5}' | sed 's/%//')
if [ "$disk_usage" -lt 80 ]; then
    echo -e "Disk usage: ${GREEN}✅ ${disk_usage}%${NC}"
else
    echo -e "Disk usage: ${RED}❌ ${disk_usage}%${NC}"
fi

# Check memory usage
memory_usage=$(free | awk 'NR==2{printf "%.0f", $3*100/$2}')
if [ "$memory_usage" -lt 80 ]; then
    echo -e "Memory usage: ${GREEN}✅ ${memory_usage}%${NC}"
else
    echo -e "Memory usage: ${RED}❌ ${memory_usage}%${NC}"
fi

echo ""
echo "📋 Service URLs:"
echo "---------------"
echo "Frontend: http://localhost"
echo "Transaction Service Admin: http://localhost:8000/admin"
echo "Config Service Admin: http://localhost:8001/admin"
echo "API Documentation: http://localhost:8000/docs"

echo ""
echo "🔧 Useful Commands:"
echo "------------------"
echo "View logs: make logs"
echo "Restart services: make restart"
echo "Check specific service: make logs-[service]"
echo "Update services: make update"

echo ""
echo "✅ Health check completed!"