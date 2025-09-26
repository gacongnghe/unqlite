# =============================================================================
# Gnosis Safe Self-Hosting Makefile
# =============================================================================
# This Makefile provides convenient commands for managing your Gnosis Safe instance

.PHONY: help setup build start stop restart logs clean backup restore create-admin migrate check-health

# Default target
help: ## Show this help message
	@echo "Gnosis Safe Self-Hosting Commands"
	@echo "=================================="
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

# =============================================================================
# Setup and Configuration
# =============================================================================

setup: ## Initial setup - copy .env.example to .env and create directories
	@echo "Setting up Gnosis Safe environment..."
	@if [ ! -f .env ]; then \
		cp .env.example .env; \
		echo "Created .env file from .env.example"; \
		echo "Please edit .env file with your configuration before running 'make start'"; \
	else \
		echo ".env file already exists"; \
	fi
	@mkdir -p logs
	@mkdir -p nginx/ssl
	@mkdir -p backups
	@echo "Setup complete! Edit .env file and run 'make start' to begin."

check-env: ## Check if .env file exists and has required variables
	@if [ ! -f .env ]; then \
		echo "Error: .env file not found. Run 'make setup' first."; \
		exit 1; \
	fi
	@echo "Checking environment configuration..."
	@if grep -q "change_this" .env; then \
		echo "Warning: Please update the placeholder values in .env file"; \
		echo "Required changes:"; \
		echo "  - POSTGRES_PASSWORD"; \
		echo "  - DJANGO_SECRET_KEY"; \
		echo "  - ETHEREUM_NODE_URL"; \
		exit 1; \
	fi
	@echo "Environment configuration looks good!"

# =============================================================================
# Docker Operations
# =============================================================================

build: check-env ## Build all Docker images
	@echo "Building Gnosis Safe Docker images..."
	docker-compose build --no-cache

start: check-env ## Start all Gnosis Safe services
	@echo "Starting Gnosis Safe services..."
	docker-compose up -d
	@echo "Services starting... Check status with 'make logs' or 'make check-health'"

stop: ## Stop all Gnosis Safe services
	@echo "Stopping Gnosis Safe services..."
	docker-compose down

restart: stop start ## Restart all Gnosis Safe services

logs: ## Show logs from all services
	docker-compose logs -f

logs-transaction: ## Show logs from transaction service only
	docker-compose logs -f transaction-service

logs-config: ## Show logs from config service only
	docker-compose logs -f config-service

logs-gateway: ## Show logs from client gateway only
	docker-compose logs -f client-gateway

logs-react: ## Show logs from React frontend only
	docker-compose logs -f safe-react

logs-nginx: ## Show logs from Nginx only
	docker-compose logs -f nginx

# =============================================================================
# Database Operations
# =============================================================================

migrate: ## Run database migrations
	@echo "Running database migrations..."
	docker-compose exec transaction-service python manage.py migrate
	docker-compose exec config-service python src/manage.py migrate
	@echo "Migrations completed!"

create-admin: ## Create admin user for transaction service
	@echo "Creating admin user for transaction service..."
	docker-compose exec -it transaction-service python manage.py createsuperuser

create-config-admin: ## Create admin user for config service
	@echo "Creating admin user for config service..."
	docker-compose exec -it config-service python src/manage.py createsuperuser

shell-transaction: ## Open Django shell for transaction service
	docker-compose exec transaction-service python manage.py shell

shell-config: ## Open Django shell for config service
	docker-compose exec config-service python src/manage.py shell

# =============================================================================
# Maintenance and Monitoring
# =============================================================================

check-health: ## Check health status of all services
	@echo "Checking service health..."
	@echo "========================="
	@echo "Docker containers:"
	@docker-compose ps
	@echo ""
	@echo "Service endpoints:"
	@echo "Frontend: http://localhost:3000"
	@echo "Transaction Service: http://localhost:8000"
	@echo "Config Service: http://localhost:8001"
	@echo "Client Gateway: http://localhost:8002"
	@echo "Nginx Proxy: http://localhost"
	@echo ""
	@echo "Testing endpoints..."
	@curl -s -o /dev/null -w "Frontend: %{http_code}\n" http://localhost:3000 || echo "Frontend: ERROR"
	@curl -s -o /dev/null -w "Transaction Service: %{http_code}\n" http://localhost:8000/health || echo "Transaction Service: ERROR"
	@curl -s -o /dev/null -w "Config Service: %{http_code}\n" http://localhost:8001/health || echo "Config Service: ERROR"
	@curl -s -o /dev/null -w "Client Gateway: %{http_code}\n" http://localhost:8002/health || echo "Client Gateway: ERROR"
	@curl -s -o /dev/null -w "Nginx: %{http_code}\n" http://localhost/health || echo "Nginx: ERROR"

clean: ## Remove all containers, volumes, and images (WARNING: This will delete all data!)
	@echo "WARNING: This will delete all containers, volumes, and images!"
	@echo "This action cannot be undone. Press Ctrl+C to cancel or wait 10 seconds..."
	@sleep 10
	docker-compose down -v --remove-orphans
	docker system prune -a -f
	@echo "Cleanup completed!"

clean-logs: ## Clean up log files
	@echo "Cleaning up log files..."
	@rm -rf logs/*
	@echo "Log cleanup completed!"

# =============================================================================
# Backup and Restore
# =============================================================================

backup: ## Create backup of database and configuration
	@echo "Creating backup..."
	@mkdir -p backups
	@BACKUP_FILE=backups/gnosis-safe-backup-$(shell date +%Y%m%d-%H%M%S).tar.gz
	@echo "Backing up to $$BACKUP_FILE"
	@docker-compose exec postgres pg_dump -U $$(grep POSTGRES_USER .env | cut -d'=' -f2) -d $$(grep POSTGRES_DB .env | cut -d'=' -f2) > backups/database-backup-$(shell date +%Y%m%d-%H%M%S).sql
	@tar -czf $$BACKUP_FILE .env docker-compose.yml nginx/ backups/database-backup-$(shell date +%Y%m%d-%H%M%S).sql
	@echo "Backup created: $$BACKUP_FILE"

restore: ## Restore from backup (specify BACKUP_FILE variable)
	@if [ -z "$(BACKUP_FILE)" ]; then \
		echo "Error: Please specify BACKUP_FILE variable"; \
		echo "Usage: make restore BACKUP_FILE=backups/gnosis-safe-backup-YYYYMMDD-HHMMSS.tar.gz"; \
		exit 1; \
	fi
	@echo "Restoring from $(BACKUP_FILE)..."
	@tar -xzf $(BACKUP_FILE)
	@echo "Restore completed! Run 'make restart' to apply changes."

# =============================================================================
# SSL Certificate Management
# =============================================================================

generate-ssl: ## Generate self-signed SSL certificate for development
	@echo "Generating self-signed SSL certificate..."
	@openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
		-keyout nginx/ssl/key.pem \
		-out nginx/ssl/cert.pem \
		-subj "/C=US/ST=State/L=City/O=Organization/CN=localhost"
	@echo "SSL certificate generated in nginx/ssl/"

# =============================================================================
# Development Tools
# =============================================================================

dev-start: ## Start services in development mode with live logs
	docker-compose up

dev-stop: ## Stop development services
	docker-compose down

update: ## Pull latest Docker images and restart services
	@echo "Updating Gnosis Safe to latest version..."
	docker-compose pull
	docker-compose up -d
	@echo "Update completed!"

# =============================================================================
# Network Configuration
# =============================================================================

show-network: ## Show Docker network configuration
	@echo "Docker networks:"
	@docker network ls
	@echo ""
	@echo "Gnosis Safe network details:"
	@docker-compose ps --format "table {{.Name}}\t{{.Status}}\t{{.Ports}}"

# =============================================================================
# Wallet-Only Commands
# =============================================================================

wallet-only: setup ## Start Gnosis Safe in wallet-maintenance mode
	@echo "Starting Gnosis Safe in wallet-maintenance mode..."
	docker-compose -f docker-compose.wallet-only.yml up -d
	@echo "Wallet-only mode started!"
	@echo "Access your Safe wallet at: http://localhost"

wallet-stop: ## Stop wallet-only services
	@echo "Stopping wallet-only services..."
	docker-compose -f docker-compose.wallet-only.yml down

wallet-logs: ## Show logs from wallet-only services
	docker-compose -f docker-compose.wallet-only.yml logs -f

# =============================================================================
# Quick Commands
# =============================================================================

quick-start: setup start migrate create-admin ## Quick start: setup, start, migrate, and create admin
	@echo "Quick start completed!"
	@echo "Access your Gnosis Safe at: http://localhost"
	@echo "Admin interfaces:"
	@echo "  - Transaction Service: http://localhost:8000/admin"
	@echo "  - Config Service: http://localhost:8001/admin"

status: check-health ## Alias for check-health

info: ## Show system information and resource usage
	@echo "System Information:"
	@echo "=================="
	@echo "Docker version: $$(docker --version)"
	@echo "Docker Compose version: $$(docker-compose --version)"
	@echo ""
	@echo "Resource usage:"
	@docker stats --no-stream --format "table {{.Container}}\t{{.CPUPerc}}\t{{.MemUsage}}"