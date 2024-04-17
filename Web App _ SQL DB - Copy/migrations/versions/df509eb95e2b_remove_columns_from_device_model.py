"""remove columns from Device model

Revision ID: df509eb95e2b
Revises: 4e931f7417ed
Create Date: 2023-12-20 16:15:21.499389

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = 'df509eb95e2b'
down_revision = '4e931f7417ed'
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    with op.batch_alter_table('device', schema=None) as batch_op:
        batch_op.drop_constraint('device_security_token_unique', type_='unique')
        batch_op.drop_column('firmware_version')
        batch_op.drop_column('security_token')

    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    with op.batch_alter_table('device', schema=None) as batch_op:
        batch_op.add_column(sa.Column('security_token', sa.VARCHAR(length=255), nullable=True))
        batch_op.add_column(sa.Column('firmware_version', sa.VARCHAR(length=50), nullable=False))
        batch_op.create_unique_constraint('device_security_token_unique', ['security_token'])

    # ### end Alembic commands ###
