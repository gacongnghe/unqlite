# Gnosis Safe Self-Hosting Setup

This repository contains everything you need to self-host your own instance of Gnosis Safe, the popular multi-signature wallet platform.

## 🚀 Quick Start

1. **Clone and setup**:
   ```bash
   git clone <your-repo>
   cd gnosis-safe-self-host
   make setup
   ```

2. **Configure your environment**:
   Edit the `.env` file with your specific configuration (see [Configuration](#configuration) section).

3. **Start the services**:
   ```bash
   make quick-start
   ```

4. **Access your Gnosis Safe**:
   - Frontend: http://localhost
   - Admin interfaces: http://localhost:8000/admin and http://localhost:8001/admin

## 📋 Prerequisites

- **Docker & Docker Compose**: Version 20.10+ recommended
- **Server Requirements**: 
  - Minimum: 2 CPU cores, 4GB RAM, 20GB storage
  - Recommended: 4 CPU cores, 8GB RAM, 50GB storage
- **Ethereum RPC Endpoint**: Infura, Alchemy, or your own node
- **Domain & SSL Certificate**: For production deployment

## 🏗️ Architecture

This setup includes the following components:

- **Safe React Frontend**: Web interface for interacting with Safes
- **Safe Transaction Service**: Handles transaction indexing and relaying
- **Safe Config Service**: Manages configuration and settings
- **Safe Client Gateway**: API gateway for frontend communication
- **PostgreSQL**: Database for storing transaction data
- **Redis**: Caching layer for improved performance
- **Nginx**: Reverse proxy and load balancer

## ⚙️ Configuration

### Environment Variables

Copy `.env.example` to `.env` and configure the following essential variables:

```bash
# Database
POSTGRES_PASSWORD=your_secure_password

# Django
DJANGO_SECRET_KEY=your_very_long_secret_key

# Ethereum
ETHEREUM_NODE_URL=https://mainnet.infura.io/v3/YOUR_PROJECT_ID

# Domain (for production)
DJANGO_ALLOWED_HOSTS=yourdomain.com,www.yourdomain.com
```

### Contract Addresses

The default configuration includes mainnet contract addresses. For other networks, update the contract addresses in `.env`:

- `SAFE_CONTRACT_ADDRESS`: Main Safe contract
- `SAFE_PROXY_FACTORY_ADDRESS`: Proxy factory contract
- `SAFE_MULTISEND_ADDRESS`: MultiSend contract
- And more...

## 🛠️ Management Commands

Use the provided Makefile for easy management:

```bash
# Basic operations
make start          # Start all services
make stop           # Stop all services
make restart        # Restart all services
make logs           # View logs

# Database operations
make migrate        # Run database migrations
make create-admin   # Create admin user
make backup         # Backup database and config

# Monitoring
make check-health   # Check service health
make status         # Show service status

# Maintenance
make clean          # Clean up everything (WARNING: deletes data!)
make update         # Update to latest version
```

## 🌐 Network Configuration

### Default Ports

- **80/443**: Nginx (main access point)
- **3000**: Safe React Frontend
- **8000**: Transaction Service API
- **8001**: Config Service API
- **8002**: Client Gateway API
- **5432**: PostgreSQL Database
- **6379**: Redis Cache

### Production Setup

For production deployment:

1. **SSL Configuration**:
   ```bash
   make generate-ssl  # For self-signed (development)
   # OR manually place your SSL certificates in nginx/ssl/
   ```

2. **Update Nginx configuration**:
   - Uncomment SSL settings in `nginx/nginx.conf`
   - Update server names and domains
   - Configure proper CORS headers

3. **Environment Variables**:
   ```bash
   DJANGO_DEBUG=False
   DJANGO_ALLOWED_HOSTS=yourdomain.com
   REACT_APP_IS_PRODUCTION=true
   ```

## 🔧 Troubleshooting

### Common Issues

1. **Services not starting**:
   ```bash
   make logs  # Check logs for errors
   make check-health  # Verify service status
   ```

2. **Database connection issues**:
   ```bash
   docker-compose logs postgres  # Check database logs
   make migrate  # Ensure migrations are applied
   ```

3. **Frontend not loading**:
   - Verify Nginx configuration
   - Check if all backend services are running
   - Ensure environment variables are properly set

### Logs and Debugging

```bash
# View all logs
make logs

# View specific service logs
make logs-transaction
make logs-config
make logs-gateway
make logs-react
make logs-nginx

# Check service health
make check-health
```

## 🔒 Security Considerations

### Production Security

1. **Change default passwords**:
   - Update `POSTGRES_PASSWORD` in `.env`
   - Generate a strong `DJANGO_SECRET_KEY`

2. **Network security**:
   - Use firewall rules to restrict access
   - Consider VPN access for admin interfaces
   - Enable SSL/TLS encryption

3. **Access control**:
   - Restrict admin interface access in `nginx.conf`
   - Use strong authentication for admin users
   - Regular security updates

### Backup Strategy

```bash
# Create regular backups
make backup

# Restore from backup
make restore BACKUP_FILE=backups/your-backup.tar.gz
```

## 📊 Monitoring and Maintenance

### Health Checks

The setup includes health check endpoints:
- `http://localhost/health` - Overall health
- `http://localhost:8000/health` - Transaction service
- `http://localhost:8001/health` - Config service
- `http://localhost:8002/health` - Client gateway

### Resource Monitoring

```bash
make info  # Show system information and resource usage
```

### Updates

```bash
make update  # Pull latest images and restart
```

## 🤝 Support

### Getting Help

1. Check the logs: `make logs`
2. Verify configuration: `make check-health`
3. Review this documentation
4. Check [Gnosis Safe documentation](https://docs.gnosis.io/safe/)

### Contributing

Feel free to submit issues and enhancement requests!

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## ⚠️ Disclaimer

This is a self-hosting setup for Gnosis Safe. Use at your own risk and ensure you understand the security implications of managing your own instance. Always test thoroughly before using with real funds.

---

**Happy Safe hosting! 🔐**